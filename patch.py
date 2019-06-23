
import os
import subprocess
import argparse
from shutil import copyfile

config_file = 'config.sh'

def get_var(varname):
    CMD = f'echo $(source {config_file}; echo ${varname})'
    p = subprocess.Popen(CMD, stdout=subprocess.PIPE, shell=True, executable='/bin/bash')
    return p.stdout.readlines()[0].strip().decode('utf-8')

# Parse config file to get PG_SOURCE_DIR
pg_src_dir = get_var('PG_SOURCE_DIR')
func_dir = get_var('FUNC_DIR')

makefile = f'{pg_src_dir}/src/backend/utils/adt/Makefile'
dat_file = f'{pg_src_dir}/src/include/catalog/pg_proc.dat'

udf_file_prefix = 'similarity_join'
udf_src = f'{func_dir}/{udf_file_prefix}.c'
udf_dst = f'{pg_src_dir}/src/backend/utils/adt/{udf_file_prefix}.c'

def modify_makefile(unpatch=False):
    # The modification is patch or unpatch
    lines = []
    with open(makefile, 'r') as f:
        lines = f.readlines()

    for i, line in enumerate(lines):

        if line.startswith('OBJS'):
    
            line = line.split(' ')

            if line[0] != 'OBJS' or line[1] != '=':
                print ('Format error!')
                return 

            if line[2] == f'{udf_file_prefix}.o':
                if unpatch:
                    line.pop(2)
                    lines[i] = ' '.join(line)
                else:
                    print (f'File: {makefile} has been patched, omitted.')
                    return 
            else:
                if unpatch: 
                    print (f'File: {makefile} not patched.')
                    return 
                else: 
                    line.insert(2, f'{udf_file_prefix}.o')
                    lines[i] = ' '.join(line)
    
    # Write back modified Makefile
    with open(makefile, 'w') as f:
        f.write(''.join(lines))

def modify_dat(unpatch=False):
    # unpatch: unpatch if patched 
    lines = []
    patched = False
    patched_begin, patched_end = 0, 0
    with open(dat_file, 'r') as f:
        lines = f.readlines()

        # Check previous patching
        for i, line in enumerate(lines):
            if '# similarity join functions begin' in line:
                patched = True
                patched_begin = i

            if '# similarity join functions end' in line:
                patched = True
                patched_end = i
                # Removing following space lines 
                for j in range(i+1, len(lines)):
                    if lines[j].isspace():
                        patched_end = j
                    else: break

                break

        if patched:
            if unpatch:
                lines = lines[:patched_begin] + lines[patched_end + 1:]
            else:
                print (f'File: {dat_file} has been patched, omitted')
                return 

        else:
            if unpatch:
                print (f'File: {dat_file} not patched.')
                return 
            else: 
                for i in range (len(lines)-1, 0, -1):
                    if lines[i].startswith(']'):
                        lines.insert(i,
'''# similarity join functions begin
{ oid => '9997', descr => 'jaccard index',
  proname => 'jaccard_index', prorettype => 'float8', proargtypes => 'text text',
  prosrc => 'jaccard_index' },
{ oid => '9998', descr => 'levenshtein distance',
  proname => 'levenshtein_distance', prorettype => 'int4', proargtypes => 'text text',
  prosrc => 'levenshtein_distance' },
{ oid => '9999', descr => 'whether levenshtein distance is less than threshold',
  proname => 'levenshtein_distance_less_than', prorettype => 'bool', proargtypes => 'text text int4',
  prosrc => 'levenshtein_distance_less_than' 
},
# similarity join functions end

''')
                        break

        with open(dat_file, 'w') as f:
            f.write(''.join(lines))
    
def patch():
    copyfile(udf_src, udf_dst)
    modify_makefile()
    modify_dat()

def unpatch():
    try:
        os.remove(udf_dst)
    except OSError:
        print (f'File: {udf_dst} not found, omitted.')
    modify_makefile(unpatch=True)
    modify_dat(unpatch=True)
    


parser = argparse.ArgumentParser()
parser.add_argument('-u', '--unpatch', help='unpatch postgresql source', action='store_true')
args = parser.parse_args()

if args.unpatch:
    unpatch()
else:
    patch()

    
