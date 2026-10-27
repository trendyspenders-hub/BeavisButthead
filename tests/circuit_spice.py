#!/usr/bin/env python3
"""Compare the C++ solver with ngspice. Requires ngspice on PATH.
The netlist shares the component ledger/device equations, not the numerical solver.
This checks discretization and solver error, not hardware fidelity.
"""
import bisect,json,math,os,pathlib,subprocess,tempfile
root=pathlib.Path(__file__).resolve().parents[1]
report=[]
with tempfile.TemporaryDirectory() as tmp:
 work=pathlib.Path(tmp);probe=work/'probe'
 subprocess.run([os.environ.get('CXX','c++'),'-std=c++17','-O3',str(root/'tests/circuit_probe.cpp'),'-o',str(probe)],check=True)
 for model in (0,1):
  for level in (.01,.5,2.):
   case=work/f'{model}-{level}';case.mkdir()
   args=[str(probe),str(model),'192000',str(level)]
   deck=subprocess.check_output(args+['netlist'],text=True)
   (case/'model.cir').write_text(deck)
   run=subprocess.run([os.environ.get('NGSPICE','ngspice'),'-b','model.cir'],cwd=case,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
   if run.returncode or not (case/'transient.txt').exists():raise RuntimeError(run.stdout)
   cpp=subprocess.run(args,check=True,capture_output=True,text=True)
   assert 'failures=0' in cpp.stderr,cpp.stderr
   bias=[float(x) for x in cpp.stderr.splitlines()[0].split()[1:]]
   spice_bias=[float(x) for x in (case/'bias.txt').read_text().splitlines()[1].split()[1:]]
   dc_error=max(abs(a-b) for a,b in zip(bias,spice_bias))
   assert len(bias)==len(spice_bias) and dc_error<.001,(model,dc_error)
   reference=[[float(x) for x in line.split()] for line in (case/'transient.txt').read_text().splitlines()[1:]]
   times=[r[0] for r in reference];error=energy=peak_error=0.;count=0
   for line in cpp.stdout.splitlines():
    t,y=map(float,line.split())
    if t<.02:continue
    k=min(max(bisect.bisect_left(times,t),1),len(times)-1)
    t0,y0=reference[k-1];t1,y1=reference[k]
    ref=y0+(y1-y0)*(t-t0)/(t1-t0)
    error+=(y-ref)**2;energy+=ref**2;peak_error=max(peak_error,abs(y-ref));count+=1
   relative=math.sqrt(error/max(energy,1e-30))
   # Backward Euler at 4x is dissipative; this is a measured numerical error gate,
   # not a claim that the modeled tubes, loading or rails match any specimen.
   assert relative<.03,(model,level,relative)
   report.append(dict(model=['AB763 preamp','2203 preamp'][model],input_peak_volts=level,internal_rate=192000,
                      dc_max_error_volts=dc_error,relative_rms_error=relative,peak_error_volts=peak_error,samples=count))
print(json.dumps(report,indent=2))
