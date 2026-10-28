import io, re

s = io.open('dev/test-cw-package.ps1', encoding='utf-8-sig').read()
m = re.search(r"\$py = @'\n(.*?)\n'@", s, re.S)
code = m.group(1).replace('import re, io', 'import re, io\nimport os\nos.chdir(r"D:\\GitHub Repositories\\CW-Beautify\\源码")')
exec(compile(code, 'yamlcheck', 'exec'))
