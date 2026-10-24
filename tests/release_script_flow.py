#!/usr/bin/env python3
"""Mock command-flow regression only; does NOT perform Apple signing/notarization."""
import os, subprocess, tempfile
from pathlib import Path
script=Path(__file__).resolve().parents[1]/'scripts/sign-mac-release.command'
with tempfile.TemporaryDirectory() as tmp:
    base=Path(tmp); tools=base/'bin'; tools.mkdir(); project=base/'project'; (project/'scripts').mkdir(parents=True)
    target=project/'scripts/sign-mac-release.command'; target.write_bytes(script.read_bytes())
    (project/'docs').mkdir(); (project/'docs/START-HERE.md').write_text('Test fixture')
    build=base/'mac-build-fixture'
    for fmt in ['Components/BBIREvan.component','VST3/BBIREvan.vst3']:
        binary=build/'payload/Library/Audio/Plug-Ins'/fmt/'Contents/MacOS/BBIREvan'
        binary.parent.mkdir(parents=True); binary.write_text('fixture, not executable')
    stub='''#!/usr/bin/env python3
import os,sys,json,shutil
from pathlib import Path
name=Path(sys.argv[0]).name; args=sys.argv[1:]
with open(os.environ['FLOW_LOG'],'a') as f: f.write(name+' '+' '.join(args)+'\\n')
if name=='uname': print('Darwin')
elif name=='ditto': shutil.copytree(args[0],args[1])
elif name in ['pkgbuild','hdiutil']: Path(args[-1]).write_text('mock package')
elif name=='xcrun' and args[:2]==['notarytool','submit']: print(json.dumps({'status':os.environ['NOTARY_RESULT']}))
'''
    for name in ['uname','ditto','lipo','codesign','pkgbuild','pkgutil','xcrun','spctl','hdiutil']:
        p=tools/name; p.write_text(stub); p.chmod(0o755)
    env=dict(os.environ,PATH=str(tools)+os.pathsep+os.environ['PATH'],IREVAN_APP_IDENTITY='Developer ID Application: MOCK',IREVAN_INSTALLER_IDENTITY='Developer ID Installer: MOCK',IREVAN_NOTARY_PROFILE='MOCK')
    for result in ['Invalid','Accepted']:
        log=base/(result+'.log'); env.update(FLOW_LOG=str(log),NOTARY_RESULT=result)
        p=subprocess.run(['bash',str(target),str(build)],env=env,capture_output=True,text=True)
        calls=log.read_text()
        if result=='Invalid':
            assert p.returncode!=0
            assert 'stapler staple' not in calls and 'hdiutil' not in calls
        else:
            assert p.returncode==0,p.stderr
            assert calls.count('notarytool submit')==2
            assert calls.count('stapler staple')==2
            assert 'spctl --assess --type install' in calls
    assert all(p.read_text()=='fixture, not executable' for p in build.rglob('BBIREvan') if p.is_file())
print('PASS: MOCK signing flow stops on rejected notarization; accepted flow staples both packages and preserves input. No real signing performed.')
