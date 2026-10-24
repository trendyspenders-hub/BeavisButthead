const fs=require('fs');const assert=require('assert');const {JSDOM,VirtualConsole}=require('jsdom');
const errors=[];const v=new VirtualConsole();v.on('jsdomError',e=>errors.push(String(e)));
const dom=new JSDOM(fs.readFileSync(require('path').join(__dirname,'../preview/Beavis-And-Butthead-Preview.html'),'utf8'),{runScripts:'dangerously',virtualConsole:v});const d=dom.window.document;
function input(id,val){const e=d.querySelector(id);assert(e,id);e.value=val;e.dispatchEvent(new dom.window.Event('input'));}
for(const [v,n] of [[-20,0],[-10,1],[0,2],[10,3],[20,3]]){input('#drive',v);assert.equal(d.querySelector('#art').dataset.frame,String(n));assert(d.querySelector('#art').src.startsWith('data:image/png;base64,'));}
d.querySelector('#drawer-open').click();assert(d.querySelector('#drawer').classList.contains('open'));assert.equal(d.querySelector('#drawer-open').getAttribute('aria-expanded'),'true');assert.equal(d.activeElement.id,'drawer-close');for(const e of d.querySelectorAll('.drawerknob'))assert.equal(e.style.display,'block');
input('#threshold',-45);assert.equal(d.querySelector('#gate').value,'-45');input('#gate',-60);assert.equal(d.querySelector('#threshold').value,'-60');
d.querySelector('#pedal-gate').click();assert.equal(d.querySelector('.switch').getAttribute('aria-pressed'),'false');
d.dispatchEvent(new dom.window.KeyboardEvent('keydown',{key:'Escape',bubbles:true}));assert(!d.querySelector('#drawer').classList.contains('open'));assert.equal(d.activeElement.id,'drawer-open');
assert.equal(errors.length,0,errors.join('\n'));console.log('PASS: artwork boundaries, drawer open/close, Escape/focus return, gate value/bypass synchronization, no JS runtime errors.');dom.window.close();
