
import os
import subprocess

root = '/Daten/Fahrvideos/_NichtGesichert/'
run = '/Daten/Projekte/gopro/gpmf-parser/build/gpmf-parser-exporter'

for r, dirs, files in os.walk(root):
    # print(f'{r} -> {dirs} -> {files}')
    for f in files:
        if not f.lower().endswith('mp4'):
            continue


        last_subdir = os.path.basename(r)
        print(f'{r} -> {last_subdir} -> {f}')

        outfile = f'{last_subdir}_{f}'

        subprocess.call([run, os.path.join(r, f), f'-otmp/{outfile}', '-a', '-v'])
