(() => {
const canvas=document.getElementById('canvas'),stick=document.getElementById('stick'),knob=document.getElementById('knob');
let pointer=null,buttons=0,moveX=0,moveY=0,lastLook=null;
const call=(name,...args)=>{if(window.Module&&Module.ccall)try{Module.ccall(name,null,args.map(x=>'number'),args)}catch(e){console.warn(e)}};
const send=()=>call('SetTouch',moveX,moveY,buttons);
document.querySelectorAll('.class-card').forEach(b=>b.addEventListener('click',()=>{
 document.querySelectorAll('.class-card').forEach(x=>x.classList.remove('selected'));
 b.classList.add('selected');call('ChooseClass',Number(b.dataset.class));
}));
try {
 const saved=JSON.parse(localStorage.getItem('tagbound-save')||'{}');
 if(Number.isInteger(saved.klass)&&saved.klass>=0&&saved.klass<3) {
  const card=document.querySelector('.class-card[data-class="'+saved.klass+'"]');
  if(card){document.querySelectorAll('.class-card').forEach(x=>x.classList.remove('selected'));card.classList.add('selected')}
 }
} catch(e) {}
document.getElementById('start').addEventListener('click',()=>{call('StartGame');document.getElementById('intro').classList.add('hidden')});
document.getElementById('restart').addEventListener('click',()=>{call('ResetProgress');location.reload()});
document.querySelectorAll('.action').forEach(b=>{
 const bit=Number(b.dataset.bit);
 const down=e=>{e.preventDefault();if(b.setPointerCapture)b.setPointerCapture(e.pointerId);buttons|=bit;b.classList.add('active');send()};
 const up=e=>{e.preventDefault();buttons&=~bit;b.classList.remove('active');send()};
 b.addEventListener('pointerdown',down);b.addEventListener('pointerup',up);b.addEventListener('pointercancel',up);b.addEventListener('lostpointercapture',up);
});
stick.addEventListener('pointerdown',e=>{pointer=e.pointerId;stick.setPointerCapture(pointer);updateStick(e)});
stick.addEventListener('pointermove',e=>{if(e.pointerId===pointer)updateStick(e)});
function releaseStick(e){if(e.pointerId!==pointer)return;pointer=null;moveX=moveY=0;knob.style.transform='translate(0,0)';send()}
stick.addEventListener('pointerup',releaseStick);stick.addEventListener('pointercancel',releaseStick);
function updateStick(e){
 const r=stick.getBoundingClientRect(),dx=e.clientX-(r.left+r.width/2),dy=e.clientY-(r.top+r.height/2),limit=42,d=Math.hypot(dx,dy)||1,k=Math.min(1,limit/d),x=dx*k,y=dy*k;
 moveX=x/limit;moveY=y/limit;knob.style.transform='translate('+x+'px,'+y+'px)';send();
}
canvas.addEventListener('pointerdown',e=>{if(e.pointerType!=='mouse'){canvas.setPointerCapture(e.pointerId);lastLook={x:e.clientX,y:e.clientY,id:e.pointerId}}});
canvas.addEventListener('pointermove',e=>{
 if(!lastLook||lastLook.id!==e.pointerId)return;
 const dx=e.clientX-lastLook.x,dy=e.clientY-lastLook.y;lastLook={x:e.clientX,y:e.clientY,id:e.pointerId};call('SetLook',dx,dy);
});
['pointerup','pointercancel','pointerleave'].forEach(n=>canvas.addEventListener(n,e=>{if(lastLook&&lastLook.id===e.pointerId)lastLook=null}));
let installEvent=null;const installButtons=[document.getElementById('install'),document.getElementById('install-float')];
window.addEventListener('beforeinstallprompt',e=>{e.preventDefault();installEvent=e;installButtons.forEach(b=>b.hidden=false)});
installButtons.forEach(b=>b.addEventListener('click',async()=>{
 if(installEvent){installEvent.prompt();await installEvent.userChoice;installEvent=null;installButtons.forEach(x=>x.hidden=true)}
 else alert('iPhone: open Share in Safari, then Add to Home Screen. Android: use the browser menu and choose Install app.');
}));
window.addEventListener('appinstalled',()=>installButtons.forEach(b=>b.hidden=true));
if('serviceWorker' in navigator)window.addEventListener('load',()=>navigator.serviceWorker.register('/sw.js').catch(console.error));
const rotate=()=>document.getElementById('rotate').classList.toggle('hidden',window.innerWidth>=window.innerHeight);
window.addEventListener('resize',rotate);rotate();
})();
