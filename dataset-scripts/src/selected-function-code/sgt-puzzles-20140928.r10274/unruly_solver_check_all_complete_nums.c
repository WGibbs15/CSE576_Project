static int unruly_solver_check_all_complete_nums(game_state *state,
                                                 struct unruly_scratch *scratch)
{
    int ret = 0;

    ret +=
        unruly_solver_check_complete_nums(state, scratch->ones_rows, TRUE,
                                          scratch->zeros_rows,
                                          scratch->zeros_cols, N_ZERO);
    ret +=
        unruly_solver_check_complete_nums(state, scratch->ones_cols, FALSE,
                                          scratch->zeros_rows,
                                          scratch->zeros_cols, N_ZERO);
    ret +=
        unruly_solver_check_complete_nums(state, scratch->zeros_rows, TRUE,
                                          scratch->ones_rows,
                                          scratch->ones_cols, N_ONE);
    ret +=
        unruly_solver_check_complete_nums(state, scratch->zeros_cols, FALSE,
                                          scratch->ones_rows,
                                          scratch->ones_cols, N_ONE);

    return ret;
}