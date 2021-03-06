#!/usr/bin/env python

# Copyright 2015-2016 Samsung Electronics Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#  This file converts ./js/*.js to a C-array in ./source/jerry_targetjs.h file

import sys
import glob
import os
import re

def extractName(path):
    return os.path.splitext(os.path.basename(path))[0]

def writeLine(fo, content, indent=0):
    buf = '  ' * indent + content + '\n'
    fo.write(buf)

def regroup(l, n):
    return [ l[i:i+n] for i in range(0, len(l), n) ]

def removeComments(code):
    pattern = r'(\".*?\"|\'.*?\')|(/\*.*?\*/|//[^\r\n]*$)'
    regex = re.compile(pattern, re.MULTILINE | re.DOTALL)
    def _replacer(match):
        if match.group(2) is not None:
            return ""
        else:
            return match.group(1)
    return regex.sub(_replacer, code)

def removeWhitespaces(code):
    return re.sub('\n+', '\n', re.sub('\n +', '\n', code))


LICENSE = '''/* Copyright 2015-2016 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the \"License\");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an \"AS IS\" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is generated by js2c.py. Please do not modify.
 */

'''

HEADER = '''#ifndef JERRY_TARGETJS_H
#define JERRY_TARGETJS_H

'''

FOOTER = '''
#endif

'''

OUT_PATH = './source/'
SRC_PATH = './js/'

# argument processing
buildtype = 'release'
if len(sys.argv) >= 2:
    buildtype = sys.argv[1]

fout = open(OUT_PATH + 'jerry_targetjs.h', 'w')
fout.write(LICENSE);
fout.write(HEADER);

def exportOneFile(path, name):
    fout.write('const static char ' + name + '_n[] = "' + name + '";\n')
    fout.write('const static char ' + name + '_s[] =\n{\n')

    fin = open(path, 'r');
    code = fin.read() + '\0'

    # minimize code when release mode
    if buildtype != 'debug':
        code = removeComments(code)
        code = removeWhitespaces(code)

    for line in regroup(code, 10):
        buf = ', '.join(map(lambda ch: format(ord(ch),"#04x"), line))
        if line[-1] != '\0':
            buf += ','
        writeLine(fout, buf, 1)
    writeLine(fout, '};')
    writeLine(fout, 'const static int ' + name + '_l = ' + str(len(code)-1) + ';')
    writeLine(fout, '')

    fin.close();

def exportOneName(name):
    writeLine(fout, '{ ' + name + '_n, ' + name + '_s, ' + name + '_l }, \\', 1)

files = glob.glob(SRC_PATH + '*.js')
for path in files:
    name = extractName(path)
    exportOneFile(path, name)


NATIVE_STRUCT = '''
struct js_source_all {
  const char* name;
  const char* source;
  const int length;
};

#define DECLARE_JS_CODES \\
struct js_source_all js_codes[] = \\
{ \\
'''

fout.write(NATIVE_STRUCT)
exportOneName('main')
filenames = map(extractName, files)
for name in filenames:
    if name != 'main':
        exportOneName(name)

writeLine(fout, '{ NULL, NULL, 0 } \\', 1)
writeLine(fout, '};')

fout.write(FOOTER)
fout.close()
