#!/usr/bin/env python3
"""Standalone shared-engine tests; this does not build or validate an AAX/AU/VST3 host wrapper."""
from pathlib import Path
import subprocess,sys,shutil,importlib.util,os
root=Path(__file__).resolve().parent.parent
out=root/'smoke-build';out.mkdir(exist_ok=True)
cmake=[shutil.which('cmake')] if shutil.which('cmake') else [sys.executable,'-m','cmake']
def run(cmd):
 print('Running:', ' '.join(map(str,cmd)),flush=True)
 subprocess.run(list(map(str,cmd)),cwd=root,check=True)
run(cmake+['-S',root/'NeuralAmpModelerCore','-B',out/'core','-DCMAKE_BUILD_TYPE=Release'])
run(cmake+['--build',out/'core','--target','render','-j','2'])
cxx=os.environ.get('CXX','c++')
run([cxx,'-std=c++20','-O2','-Wall','-Wextra','-Werror',root/'tests/pedal_dsp.cpp','-o',out/'pedal-test'])
run([out/'pedal-test'])
run([cxx,'-std=c++20','-O2','-I'+str(root/'iPlug2/IPlug'),'-I'+str(root/'iPlug2/WDL'),root/'tests/state_reader.cpp','-o',out/'state-test'])
run([out/'state-test'])
objs=list((out/'core/tools/CMakeFiles/render.dir/__/NAM').rglob('*.o'))
if not objs:raise SystemExit('No NAM object files found; check the selected CMake generator.')
cmd=[cxx,'-std=c++20','-O2','-DNAM_ENABLE_A2_FAST','-I'+str(root/'NeuralAmpModelerCore'),'-I'+str(root/'eigen'),'-I'+str(root/'NeuralAmpModelerCore/Dependencies/eigen'),'-I'+str(root/'NeuralAmpModelerCore/Dependencies/nlohmann'),root/'tests/chain_smoke.cpp',root/'BBIREvan/ToneStack.cpp']
cmd += list((root/'AudioDSPTools/dsp').glob('*.cpp'))+objs+['-o',out/'chain-smoke']
run(cmd);run([out/'chain-smoke',root/'models',root/'models/cabinets'])
