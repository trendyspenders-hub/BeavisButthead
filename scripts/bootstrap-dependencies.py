#!/usr/bin/env python3
from pathlib import Path
import shutil, subprocess, tempfile
root=Path(__file__).resolve().parents[1]
commit='16be869746b8915885c7a35bafdcc0061faeb50e'
with tempfile.TemporaryDirectory(prefix='bbirevan-deps-') as tmp:
 source=Path(tmp)/'upstream'
 subprocess.run(['git','clone','--no-checkout','https://github.com/sdatkinson/NeuralAmpModelerPlugin.git',str(source)],check=True)
 subprocess.run(['git','checkout','--detach',commit],cwd=source,check=True)
 subprocess.run(['git','submodule','update','--init','--recursive'],cwd=source,check=True)
 for name in ['iPlug2','eigen','AudioDSPTools','NeuralAmpModelerCore']:
  if (root/name).exists(): raise SystemExit(f'Refusing to replace existing dependency: {name}')
  shutil.copytree(source/name,root/name,ignore=shutil.ignore_patterns('.git'),symlinks=True)
 # Original binary artwork/fonts/icons are unchanged from the pinned upstream.
 for original in (source/'NeuralAmpModeler').rglob('*'):
  if not original.is_file(): continue
  destination=root/'BBIREvan'/original.relative_to(source/'NeuralAmpModeler')
  try: original.read_bytes().decode('utf-8')
  except UnicodeDecodeError:
   if not destination.exists():
    destination.parent.mkdir(parents=True,exist_ok=True)
    shutil.copy2(original,destination)
for patch in (root/'.vendor-overrides').rglob('*'):
 if patch.is_file():
  dest=root/patch.relative_to(root/'.vendor-overrides'); dest.parent.mkdir(parents=True,exist_ok=True); shutil.copy2(patch,dest)
print('Pinned engine dependencies and original resources prepared.')
