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
const musicNotes=[146.83,174.61,220.00,261.63,220.00,174.61,196.00,220.00,293.66,261.63,220.00,196.00,174.61,196.00,220.00,174.61];
let musicStep=0,musicTimer=null,musicRequested=false,musicPaused=false,musicNotesPlayed=0;
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
function playMusicNote(){
 try {
  if(!audioEnabled||!audioContext||audioContext.state!=='running')return false;
  const now=audioContext.currentTime,note=musicNotes[musicStep];
  musicStep=(musicStep+1)%musicNotes.length;
  const fundamental=audioContext.createOscillator();
  const overtone=audioContext.createOscillator();
  const filter=audioContext.createBiquadFilter();
  const gain=audioContext.createGain();
  filter.type='lowpass';filter.frequency.setValueAtTime(900,now);filter.Q.setValueAtTime(0.35,now);
  fundamental.type='triangle';fundamental.frequency.setValueAtTime(note,now);
  overtone.type='sine';overtone.frequency.setValueAtTime(note*2,now);
  gain.gain.setValueAtTime(0.0001,now);
  gain.gain.exponentialRampToValueAtTime(0.008,now+0.025);
  gain.gain.exponentialRampToValueAtTime(0.0001,now+0.39);
  fundamental.connect(filter);overtone.connect(filter);filter.connect(gain);gain.connect(audioContext.destination);
  fundamental.start(now);overtone.start(now);
  fundamental.stop(now+0.41);overtone.stop(now+0.41);
  musicNotesPlayed++;
  return true;
 } catch(e) { return false; }
}
function beginMusicLoop(){
 if(!musicRequested||musicPaused||!audioEnabled||musicTimer!==null)return false;
 if(!initAudio())return false;
 if(audioContext.state!=='running'){
  audioContext.resume().then(()=>beginMusicLoop()).catch(()=>{});
  return false;
 }
 playMusicNote();
 musicTimer=window.setInterval(playMusicNote,460);
 return true;
}
function startMusic(){
 musicRequested=true;musicPaused=false;
 return beginMusicLoop();
}
function pauseMusic(){
 musicPaused=true;
 if(musicTimer!==null){window.clearInterval(musicTimer);musicTimer=null}
}
function resumeMusic(){
 if(!musicRequested)return false;
 musicPaused=false;
 return beginMusicLoop();
}
function setAudioEnabled(enabled){
 audioEnabled=!!enabled;
 try { localStorage.setItem('tagbound-sound',audioEnabled?'on':'off'); } catch(e) {}
 syncSoundButtons();
 if(audioEnabled){
  initAudio();
  if(musicRequested&&!musicPaused)beginMusicLoop();
 }else{
  if(musicTimer!==null){window.clearInterval(musicTimer);musicTimer=null}
  if(audioContext?.state==='running')audioContext.suspend().catch(()=>{});
 }
 return audioEnabled;
}
const soundToggle=document.getElementById('sound-toggle');
const settingsSoundToggle=document.getElementById('settings-sound');
function syncSoundButtons(){
 if(soundToggle){
  soundToggle.textContent='SOUND: '+(audioEnabled?'ON':'OFF');
  soundToggle.setAttribute('aria-pressed',String(audioEnabled));
 }
 if(settingsSoundToggle){
  settingsSoundToggle.classList.toggle('muted',!audioEnabled);
  settingsSoundToggle.setAttribute('aria-label',audioEnabled?'Mute sound':'Enable sound');
  settingsSoundToggle.setAttribute('aria-pressed',String(audioEnabled));
 }
}
try { audioEnabled=localStorage.getItem('tagbound-sound')!=='off'; } catch(e) {}
window.TagboundAudio={
 init:initAudio,play:playAudio,getState:()=>audioContext?.state||'uninitialized',
 setEnabled:setAudioEnabled,startMusic,pauseMusic,resumeMusic,
 getMusicState:()=>musicTimer!==null?'playing':musicRequested?(musicPaused?'paused':'stopped'):'stopped',
 getMusicNoteCount:()=>musicNotesPlayed
};
syncSoundButtons();
[soundToggle,settingsSoundToggle].filter(Boolean).forEach(button=>button.addEventListener('click',()=>setAudioEnabled(!audioEnabled)));
const call=(name,...args)=>{if(window.Module&&Module.ccall)try{Module.ccall(name,null,args.map(x=>'number'),args)}catch(e){console.warn(e)}};
const send=()=>call('SetTouch',moveX,moveY,buttons);
const classCards=[...document.querySelectorAll('.class-card')];
const bowAction=document.querySelector('.action[data-bit="16"]');
let selectedClass=0;
function syncClassControls(){
 if(bowAction)bowAction.hidden=selectedClass!==1;
}
classCards.forEach(button=>button.addEventListener('click',()=>{
 classCards.forEach(card=>card.classList.remove('selected'));
 button.classList.add('selected');
 selectedClass=Number(button.dataset.class);
 call('ChooseClass',selectedClass);
 syncClassControls();
}));
try {
 const saved=JSON.parse(localStorage.getItem('tagbound-save')||'{}');
 if(Number.isInteger(saved.klass)&&saved.klass>=0&&saved.klass<3) {
  selectedClass=saved.klass;
  const card=document.querySelector('.class-card[data-class="'+selectedClass+'"]');
  if(card){classCards.forEach(x=>x.classList.remove('selected'));card.classList.add('selected')}
 }
} catch(e) {}
syncClassControls();
const intro=document.getElementById('intro');
const ending=document.getElementById('ending');
const settings=document.getElementById('settings');
const settingsOpenButton=document.getElementById('settings-open');
const touchControls=document.getElementById('touch-controls');
const resumeButton=document.getElementById('resume');
function resetTouchInput(){
 const stickPointer=pointer,lookPointer=lastLook?.id;
 pointer=null;lastLook=null;buttons=0;moveX=moveY=0;
 if(stickPointer!==null&&stick.hasPointerCapture(stickPointer))stick.releasePointerCapture(stickPointer);
 if(lookPointer!==undefined&&canvas.hasPointerCapture(lookPointer))canvas.releasePointerCapture(lookPointer);
 document.querySelectorAll('.action').forEach(button=>{
  const id=button.dataset.pointerId;
  if(id!==undefined&&button.hasPointerCapture(Number(id)))button.releasePointerCapture(Number(id));
  button.classList.remove('active');
  delete button.dataset.pointerId;
 });
 call('ResetLook');
 knob.style.transform='translate(0,0)';
 send();
}
function openSettings(){
 resetTouchInput();
 window.TagboundAudio?.pauseMusic?.();
 settings.classList.remove('hidden');
 settingsOpenButton.classList.add('hidden');
 settingsOpenButton.setAttribute('aria-expanded','true');
 touchControls.classList.add('hidden');
 resumeButton.focus();
}
function closeSettings(){
 settings.classList.add('hidden');
 window.TagboundAudio?.resumeMusic?.();
 touchControls.classList.remove('hidden');
 settingsOpenButton.setAttribute('aria-expanded','false');
 if(intro.classList.contains('hidden')&&ending.classList.contains('hidden'))settingsOpenButton.classList.remove('hidden');
 canvas.focus();
}
settingsOpenButton.addEventListener('click',openSettings);
resumeButton.addEventListener('click',closeSettings);
document.addEventListener('keydown',event=>{
 if(event.key==='Escape'&&!settings.classList.contains('hidden'))closeSettings();
});
window.TagboundSettings={isOpen:()=>!settings.classList.contains('hidden')};
document.getElementById('start').addEventListener('click',()=>{
 call('StartGame');
 if(ending.classList.contains('hidden'))window.TagboundAudio?.startMusic?.();
 intro.classList.add('hidden');
 if(ending.classList.contains('hidden'))settingsOpenButton.classList.remove('hidden');
});
document.getElementById('restart').addEventListener('click',()=>{call('ResetProgress');location.reload()});
document.querySelectorAll('.action').forEach(b=>{
 const bit=Number(b.dataset.bit);
 const down=e=>{e.preventDefault();if(b.setPointerCapture)b.setPointerCapture(e.pointerId);b.dataset.pointerId=String(e.pointerId);buttons|=bit;b.classList.add('active');send()};
 const up=e=>{e.preventDefault();buttons&=~bit;b.classList.remove('active');delete b.dataset.pointerId;send()};
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
