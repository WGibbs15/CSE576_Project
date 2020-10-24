static int
s390_prelink_conflict_rela (DSO *dso, struct prelink_info *info,
			    GElf_Rela *rela, GElf_Addr relaaddr)
{
  GElf_Addr value;
  struct prelink_conflict *conflict;
  struct prelink_tls *tls;
  GElf_Rela *ret;

  if (GELF_R_TYPE (rela->r_info) == R_390_RELATIVE
      || GELF_R_TYPE (rela->r_info) == R_390_NONE
      || info->dso == dso)
    /* Fast path: nothing to do.  */
    return 0;
  conflict = prelink_conflict (info, GELF_R_SYM (rela->r_info),
			       GELF_R_TYPE (rela->r_info));
  if (conflict == NULL)
    {
      if (info->curtls == NULL)
	return 0;
      switch (GELF_R_TYPE (rela->r_info))
	{
	/* Even local DTPMOD and TPOFF relocs need conflicts.  */
	case R_390_TLS_DTPMOD:
	case R_390_TLS_TPOFF:
	/* IRELATIVE always need conflicts.  */
	case R_390_IRELATIVE:
	  break;
	default:
	  return 0;
	}
      value = 0;
    }
  else
    {
      /* DTPOFF wants to see only real conflicts, not lookups
	 with reloc_class RTYPE_CLASS_TLS.  */
      if (GELF_R_TYPE (rela->r_info) == R_390_TLS_DTPOFF
	  && conflict->lookup.tls == conflict->conflict.tls
	  && conflict->lookupval == conflict->conflictval)
	return 0;

      value = conflict_lookup_value (conflict);
    }
  ret = prelink_conflict_add_rela (info);
  if (ret == NULL)
    return 1;
  ret->r_offset = rela->r_offset;
  ret->r_info = GELF_R_INFO (0, R_390_32);
  value += rela->r_addend;
  switch (GELF_R_TYPE (rela->r_info))
    {
    case R_390_GLOB_DAT:
    case R_390_JMP_SLOT:
      ret->r_addend = (Elf32_Sword) (value - rela->r_addend);
      if (conflict != NULL && conflict->ifunc)
	ret->r_info = GELF_R_INFO (0, R_390_IRELATIVE);
      break;
    case R_390_32:
    case R_390_IRELATIVE:
      ret->r_addend = (Elf32_Sword) value;
      if (conflict != NULL && conflict->ifunc)
	ret->r_info = GELF_R_INFO (0, R_390_IRELATIVE);
      break;
    case R_390_PC32:
      ret->r_addend = (Elf32_Sword) (value - rela->r_offset);
      break;
    case R_390_PC32DBL:
    case R_390_PLT32DBL:
      ret->r_addend = ((Elf32_Sword) (value - rela->r_offset)) >> 1;
      break;
    case R_390_PC16:
      value -= rela->r_offset;
    case R_390_16:
      ret->r_addend = (Elf32_Half) value;
      ret->r_info = GELF_R_INFO (0, R_390_16);
      break;
    case R_390_PC16DBL:
    case R_390_PLT16DBL:
      ret->r_addend = (Elf32_Half) (((int16_t) (value - rela->r_offset)) >> 1);
      ret->r_info = GELF_R_INFO (0, R_390_16);
      break;
    case R_390_8:
      ret->r_addend = value & 0xff;
      ret->r_info = GELF_R_INFO (0, R_390_8);
      break;
    case R_390_COPY:
      error (0, 0, "R_390_COPY should not be present in shared libraries");
      return 1;
    case R_390_TLS_DTPMOD:
    case R_390_TLS_DTPOFF:
    case R_390_TLS_TPOFF:
      if (conflict != NULL
	  && (conflict->reloc_class != RTYPE_CLASS_TLS
	      || conflict->lookup.tls == NULL))
	{
	  error (0, 0, "%s: TLS reloc not resolving to STT_TLS symbol",
		 dso->filename);
	  return 1;
	}
      tls = conflict ? conflict->lookup.tls : info->curtls;
      switch (GELF_R_TYPE (rela->r_info))
	{
	case R_390_TLS_DTPMOD:
	  ret->r_addend = tls->modid;
	  break;
	case R_390_TLS_DTPOFF:
	  ret->r_addend = value;
	  break;
	case R_390_TLS_TPOFF:
	  ret->r_addend = value - tls->offset;
	  break;
	}
      break;

    default:
      error (0, 0, "%s: Unknown S390 relocation type %d", dso->filename,
	     (int) GELF_R_TYPE (rela->r_info));
      return 1;
    }
  return 0;
}