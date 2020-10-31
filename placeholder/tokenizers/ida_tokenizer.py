import os
import json
import hashlib
import tempfile
import subprocess

from .c_tokenizer import CTokenizer

################## CONFIG #######################
IDA_PATH = "/home/kylebot/src/ida/ida-7.5sp2-py38-tokenizer"
BEGIN_BANNER = "===================================IDA SCRIPT START============================================="
END_BANNER = "===================================IDA SCRIPT END============================================="


################## INIT #######################
IDA_BIN = os.path.join(IDA_PATH, "ida64")
SCRIPT_PATH = os.path.join(os.path.dirname(os.path.realpath(__file__)), "utils", "idascript.py")
CACHE_DIR = os.path.join("/tmp", "ida_tokenizer_cache")
assert os.path.exists(IDA_PATH), f"IDA_PATH: {IDA_PATH} does not exists"

# make sure IDA is runnable
output = subprocess.getoutput("ps aux | grep ./lmgrd | grep -v grep")
assert "lmgrd" in output, "Launch your License Manager Daemon for IDA first."

class IDATokenizer:
    def __init__(self):
        self.ctokenizer = CTokenizer()
        if not os.path.exists(CACHE_DIR):
            os.mkdir(CACHE_DIR)

    def _get_key(self, bin_path):
        path = os.path.abspath(bin_path)
        h = hashlib.sha256(os.path.abspath(bin_path).encode()).digest().hex()
        bin_name = os.path.basename(path)
        pkg_name = os.path.basename(os.path.dirname(path))
        return f"{pkg_name}_{bin_name}_{h}"

    def _load_cache(self, bin_path):
        key = self._get_key(bin_path)
        cache_path = os.path.join(CACHE_DIR, key)
        if not os.path.exists(cache_path):
            return None
        with open(cache_path, 'r') as f:
            return json.load(f)

    def _save_cache(self, bin_path, info):
        key = self._get_key(bin_path)
        cache_path = os.path.join(CACHE_DIR, key)
        assert not os.path.exists(cache_path)
        with open(cache_path, 'w') as f:
            json.dump(info, f)

    def _extract_bin_info(self, bin_path):
        # run IDA to extract its log
        cmd = [IDA_BIN, '-B', '-S'+SCRIPT_PATH, os.path.abspath(bin_path)]
        with tempfile.NamedTemporaryFile(mode='r', dir="/tmp", prefix='ida_tokenizer-') as f:
            log_name = f.name
            env = os.environ
            env["IDALOG"] = f.name
            subprocess.run(cmd, env=env)
            assert os.path.getsize(log_name) != 0, cmd
            content = f.read()

        # locate the log generated by out idascript.py
        assert BEGIN_BANNER in content and END_BANNER in content, content
        output = content.split(BEGIN_BANNER)[1].split(END_BANNER)[0]

        # locate the json output
        lines = output.splitlines()
        idx = lines.index('JSON:')
        info = json.loads(lines[idx+1])
        print(f"binary info extraction elapsed in {info['time']} seconds")
        return info

    def get_func_info(self, bin_path, func_name):
        info = self._load_cache(bin_path)

        # try to load cached info
        if not info:
            info = self._extract_bin_info(bin_path)
            self._save_cache(bin_path, info)

        data = info['data']

        # now return the decompiled code
        assert func_name in data, f"{func_name} does not exists in the binary {bin_path}!"
        return data[func_name]

    def tokenize(self, bin_path, func_name):
        info = self.get_func_info(bin_path, func_name)
        code = info['code']
        return self.ctokenizer.tokenize(code)

    def detokenize(self, tokens):
        return self.ctokenizer.detokenize(tokens)

if __name__ == "__main__":
    tokenizer = IDATokenizer()
    tokenizer.tokenize("tests/exec_files/test_1", "main")
