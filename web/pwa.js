(() => {
const canvas=document.getElementById('canvas'),stick=document.getElementById('stick'),knob=document.getElementById('knob');
let pointer=null,buttons=0,moveX=0,moveY=0,lastLook=null;
let audioContext=null,audioEnabled=true;
const soundProfiles=[
 {wave:'sawtooth',start:520,end:115,duration:0.13,volume:0.035,cutoff:1800},
 {wave:'triangle',start:980,end:280,duration:0.11,volume:0.03,cutoff:2400},
 {wave:'sawtooth',start:180,end:48,duration:0.19,volume:0.04,cutoff:850},
 {wave:'sine',start:145,end:285,duration:0.10,volume:0.032,cutoff:1100},
 {wave:'square',start:105,end:42,duration:0.15,volume:0.04,cutoff:700},
 {wave:'triangle',start:82,end:54,duration:0.34,volume:0.038,cutoff:520},
 {wave:'sine',start:320,end:760,duration:0.25,volume:0.032,cutoff:1700},
 {wave:'triangle',start:680,end:410,duration:0.14,volume:0.024,cutoff:1900},
 {wave:'triangle',start:1450,end:520,duration:0.09,volume:0.03,cutoff:3600}
];
function initAudio(){
 try {
  if(!audioEnabled)return false;
  const AudioContextClass=window.AudioContext||window.webkitAudioContext;
  if(!AudioContextClass)return false;
  if(!audioContext)audioContext=new AudioContextClass();
  if(audioContext.state==='suspended')audioContext.resume().catch(()=>{});
  return true;
 } catch(e) { return false; }
}
function playAudio(kind){
 try {
  if(!audioEnabled)return false;
  const sound=soundProfiles[kind];
  if(!sound||!audioContext||audioContext.state==='closed')return false;
  if(audioContext.state==='suspended')audioContext.resume().catch(()=>{});
  const now=audioContext.currentTime;
  const oscillator=audioContext.createOscillator();
  const filter=audioContext.createBiquadFilter();
  const gain=audioContext.createGain();
  filter.type='lowpass';filter.frequency.setValueAtTime(sound.cutoff,now);
  oscillator.type=sound.wave;
  oscillator.frequency.setValueAtTime(sound.start,now);
  oscillator.frequency.exponentialRampToValueAtTime(sound.end,now+sound.duration);
  gain.gain.setValueAtTime(0.0001,now);
  gain.gain.exponentialRampToValueAtTime(sound.volume,now+0.008);
  gain.gain.exponentialRampToValueAtTime(0.0001,now+sound.duration);
  oscillator.connect(filter);filter.connect(gain);gain.connect(audioContext.destination);
  oscillator.start(now);oscillator.stop(now+sound.duration+0.01);
  return true;
 } catch(e) { return false; }
}
function setAudioEnabled(enabled){
 audioEnabled=!!enabled;
 try { localStorage.setItem('tagbound-sound',audioEnabled?'on':'off'); } catch(e) {}
 syncSoundButtons();
 if(audioEnabled)initAudio();
 else if(audioContext?.state==='running')audioContext.suspend().catch(()=>{});
 return audioEnabled;
}
const soundToggle=document.getElementById('sound-toggle');
const settingsSoundToggle=document.getElementById('settings-sound');
const soundButtons=[soundToggle,settingsSoundToggle].filter(Boolean);
function syncSoundButtons(){
 soundButtons.forEach(button=>{
  button.textContent='SOUND: '+(audioEnabled?'ON':'OFF');
  button.setAttribute('aria-pressed',String(audioEnabled));
 });
}
try { audioEnabled=localStorage.getItem('tagbound-sound')!=='off'; } catch(e) {}
window.TagboundAudio={init:initAudio,play:playAudio,getState:()=>audioContext?.state||'uninitialized',setEnabled:setAudioEnabled};
syncSoundButtons();
soundButtons.forEach(button=>button.addEventListener('click',()=>setAudioEnabled(!audioEnabled)));
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
const intro=document.getElementById('intro');
const ending=document.getElementById('ending');
const settings=document.getElementById('settings');
const settingsOpenButton=document.getElementById('settings-open');
const touchControls=document.getElementById('touch-controls');
const touchHelp=document.getElementById('touch-help');
const resumeButton=document.getElementById('resume');
function openSettings(){
 settings.classList.remove('hidden');
 settingsOpenButton.classList.add('hidden');
 touchControls.classList.add('hidden');
 touchHelp.classList.add('hidden');
 resumeButton.focus();
}
function closeSettings(){
 settings.classList.add('hidden');
 if(intro.classList.contains('hidden')&&ending.classList.contains('hidden'))settingsOpenButton.classList.remove('hidden');
 settingsOpenButton.focus();
}
settingsOpenButton.addEventListener('click',openSettings);
resumeButton.addEventListener('click',closeSettings);
document.addEventListener('keydown',event=>{
 if(event.key==='Escape'&&!settings.classList.contains('hidden'))closeSettings();
});
window.TagboundSettings={isOpen:()=>!settings.classList.contains('hidden')};
document.getElementById('start').addEventListener('click',()=>{
 call('StartGame');
 intro.classList.add('hidden');
 if(ending.classList.contains('hidden'))settingsOpenButton.classList.remove('hidden');
});
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
let installEvent=null;
const installButtons=[document.getElementById('install'),document.getElementById('settings-install')].filter(Boolean);
function updateInstallButtons(){
 const installed=window.matchMedia('(display-mode: standalone)').matches||navigator.standalone===true;
 installButtons.forEach(button=>{button.hidden=installed});
}
async function requestInstall(){
 if(installEvent){
  const event=installEvent;
  installEvent=null;
  await event.prompt();
  await event.userChoice;
  updateInstallButtons();
 }else{
  alert('iPhone: open Share in Safari, then Add to Home Screen. Android: use the browser menu and choose Install app.');
 }
}
updateInstallButtons();
window.addEventListener('beforeinstallprompt',event=>{event.preventDefault();installEvent=event});
installButtons.forEach(button=>button.addEventListener('click',requestInstall));
window.addEventListener('appinstalled',()=>{installButtons.forEach(button=>{button.hidden=true})});
if('serviceWorker' in navigator)window.addEventListener('load',()=>navigator.serviceWorker.register('/sw.js').catch(console.error));
const rotate=()=>document.getElementById('rotate').classList.toggle('hidden',window.innerWidth>=window.innerHeight);
window.addEventListener('resize',rotate);rotate();
})();
