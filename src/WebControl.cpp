#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "WebControl.h"
#include "Moteur.h"
#include "Accessoires.h"
#include "Batterie.h"
#include "Capteur_IR.h"

const char* apName     = "SMD-Control";
const char* apPassword = "12345678";

WebServer server(80);

ModeRobot modeRobot = MODE_MANUEL;

String checked(bool state) {
  return state ? "checked" : "";
}

String modeTexte() {
  if (modeRobot == MODE_AUTO) return "Automatique";
  if (modeRobot == MODE_RETOUR_BASE) return "Retour base";
  return "Manuel";
}

String signalTexte() {
  if (stationSignalConfirme == STATION_VU) return "VU";
  return "AUCUN";
}

String pageHTML() {
  float tension = lireTensionBatterie();
  int pct = lirePourcentageBatterie(tension);

  String autoActive   = (modeRobot == MODE_AUTO)       ? "active" : "";
  String manualActive = (modeRobot == MODE_MANUEL)      ? "active" : "";
  String retourActive = (modeRobot == MODE_RETOUR_BASE) ? "active" : "";

  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>SuckMyDust</title>
<style>
*{box-sizing:border-box}
body{margin:0;font-family:Arial;background:linear-gradient(135deg,#eef3ff,#dbe4ff);color:#0f172a}
.app{max-width:430px;margin:auto;min-height:100vh;padding:18px}
.header{display:flex;justify-content:center;align-items:center;margin-bottom:18px}
.logo{font-size:28px;font-weight:900}
.logo span{color:#2563eb}
.card{background:rgba(255,255,255,.88);border-radius:28px;padding:20px;margin-bottom:16px;box-shadow:0 20px 45px #64748b33}
.robot{text-align:center}
.robotCircle{width:185px;height:185px;margin:10px auto;border-radius:50%;background:linear-gradient(145deg,#fff,#dbeafe);box-shadow:inset 0 0 0 12px #f8fafc,0 18px 35px #94a3b844;position:relative}
.robotCircle:before{content:"";position:absolute;width:52px;height:52px;background:#e2e8f0;border-radius:50%;top:58px;left:66px;box-shadow:inset 0 0 0 8px white}
.robotCircle:after{content:"";position:absolute;width:70px;height:10px;background:#0f172a;border-radius:20px;bottom:32px;left:58px}
.battery{font-size:36px;font-weight:900;color:#2563eb}
.batteryText{color:#64748b;font-size:14px}
.bar{width:100%;height:14px;background:#e2e8f0;border-radius:20px;overflow:hidden;margin-top:12px}
.fill{height:100%;background:linear-gradient(90deg,#22c55e,#2563eb);width:)rawliteral" + String(pct) + R"rawliteral(%;}
.modeGrid{display:grid;grid-template-columns:1fr 1fr;gap:12px}
.modeBtn{padding:18px;border:0;border-radius:22px;background:#f8fafc;font-size:16px;font-weight:800;color:#64748b;box-shadow:inset 0 0 0 1px #e2e8f0}
.modeBtn.active{background:linear-gradient(135deg,#2563eb,#1d4ed8);color:white;box-shadow:0 12px 24px #2563eb55}
.modeBtn.big{grid-column:span 2}
.controlGrid{display:grid;grid-template-columns:1fr 1fr;gap:12px}
.btn{border:0;border-radius:22px;padding:18px;font-size:16px;font-weight:800;color:white;background:#2563eb;box-shadow:0 12px 24px #2563eb44}
.btn.big{grid-column:span 2}
.btn.stop{background:#ef4444;box-shadow:0 12px 24px #ef444444}
.modeStatus{font-size:16px;color:#64748b;margin-top:-5px;margin-bottom:14px}
.modeStatus span{font-weight:900;color:#2563eb}
h2{margin:0 0 14px;font-size:20px}
.accessory{display:flex;justify-content:space-between;align-items:center;padding:14px 0;border-bottom:1px solid #e2e8f0}
.accessory:last-child{border-bottom:0}
.accName{font-weight:800}
.accState{font-size:13px;color:#64748b}
.switch{position:relative;width:64px;height:34px}
.switch input{display:none}
.slider{position:absolute;inset:0;background:#cbd5e1;border-radius:999px;transition:.2s}
.slider:before{content:"";position:absolute;width:28px;height:28px;left:3px;top:3px;background:white;border-radius:50%;transition:.2s;box-shadow:0 4px 10px #0003}
input:checked + .slider{background:#2563eb}
input:checked + .slider:before{transform:translateX(30px)}
.debug{background:#0f172a;border-radius:28px;padding:20px;margin-bottom:16px}
.debug h2{color:white;margin:0 0 14px}
.debugRow{display:flex;justify-content:space-between;align-items:center;padding:10px 0;border-bottom:1px solid #1e293b}
.debugRow:last-child{border-bottom:0}
.debugLabel{color:#94a3b8;font-size:14px}
.debugVal{font-weight:900;font-size:15px;color:white}
.debugVal.vu{color:#22c55e}
.debugVal.aucun{color:#ef4444}
.rawBlock{margin-top:12px;padding:12px;background:#1e293b;border-radius:16px}
.rawTitle{color:#94a3b8;font-size:12px;margin-bottom:8px;font-weight:700;text-transform:uppercase}
.rawGrid{display:grid;grid-template-columns:1fr 1fr;gap:8px}
.rawItem{background:#0f172a;border-radius:10px;padding:8px 10px}
.rawItemLabel{color:#475569;font-size:11px}
.rawItemVal{color:white;font-weight:900;font-size:15px}
</style>
</head>
<body>
<div class="app">

<div class="header">
  <div class="logo">Suck<span>My</span>Dust</div>
</div>

<div class="card robot">
  <div class="robotCircle"></div>
  <div class="battery">)rawliteral" + String(pct) + R"rawliteral(%</div>
  <div class="batteryText">Batterie - )rawliteral" + String(tension, 2) + R"rawliteral( V</div>
  <div class="bar"><div class="fill"></div></div>
</div>

<div class="debug">
  <h2>Debug IR</h2>
  <div class="debugRow">
    <div class="debugLabel">Signal confirme</div>
    <div class="debugVal aucun" id="dbSignal">--</div>
  </div>
  <div class="debugRow">
    <div class="debugLabel">Dernier signal</div>
    <div class="debugVal" id="dbTemps">--</div>
  </div>
  <div class="debugRow">
    <div class="debugLabel">Mode robot</div>
    <div class="debugVal" id="dbMode">--</div>
  </div>
  <div class="rawBlock">
    <div class="rawTitle">Valeurs brutes capteur</div>
    <div class="rawGrid">
      <div class="rawItem">
        <div class="rawItemLabel">rawlen</div>
        <div class="rawItemVal" id="dbRawlen">--</div>
      </div>
      <div class="rawItem">
        <div class="rawItemLabel">nbBursts</div>
        <div class="rawItemVal" id="dbBursts">--</div>
      </div>
      <div class="rawItem">
        <div class="rawItemLabel">t1 (us)</div>
        <div class="rawItemVal" id="dbT1">--</div>
      </div>
      <div class="rawItem">
        <div class="rawItemLabel">t2 (us)</div>
        <div class="rawItemVal" id="dbT2">--</div>
      </div>
      <div class="rawItem">
        <div class="rawItemLabel">t3 (us)</div>
        <div class="rawItemVal" id="dbT3">--</div>
      </div>
      <div class="rawItem">
        <div class="rawItemLabel">total (us)</div>
        <div class="rawItemVal" id="dbTotal">--</div>
      </div>
    </div>
  </div>
</div>

<div class="card">
  <h2>Mode de nettoyage</h2>
  <div class="modeStatus">Mode actuel : <span id="modeText">)rawliteral" + modeTexte() + R"rawliteral(</span></div>
  <div class="modeGrid">
    <button id="btnManuel" class="modeBtn )rawliteral" + manualActive + R"rawliteral(" onclick="cmd('/mode_manuel')">Manuel</button>
    <button id="btnAuto"   class="modeBtn )rawliteral" + autoActive   + R"rawliteral(" onclick="cmd('/mode_auto')">Automatique</button>
    <button id="btnRetour" class="modeBtn big )rawliteral" + retourActive + R"rawliteral(" onclick="cmd('/retour_base')">Retour base</button>
  </div>
</div>

<div class="card">
  <h2>Deplacement</h2>
  <div class="controlGrid">
    <button class="btn big"      onclick="cmd('/avance')">Avancer</button>
    <button class="btn"          onclick="cmd('/gauche')">Gauche</button>
    <button class="btn"          onclick="cmd('/droite')">Droite</button>
    <button class="btn big"      onclick="cmd('/recule')">Reculer</button>
    <button class="btn stop big" onclick="cmd('/stop')">Arret</button>
  </div>
</div>

<div class="card">
  <h2>Accessoires</h2>
  <div class="accessory">
    <div><div class="accName">Aspirateur</div><div class="accState">Moteur aspiration</div></div>
    <label class="switch"><input type="checkbox" )rawliteral" + checked(vacState) + R"rawliteral( onchange="toggle('/vac_', this.checked)"><span class="slider"></span></label>
  </div>
  <div class="accessory">
    <div><div class="accName">Brosse centrale</div><div class="accState">Nettoyage principal</div></div>
    <label class="switch"><input type="checkbox" )rawliteral" + checked(mainState) + R"rawliteral( onchange="toggle('/main_', this.checked)"><span class="slider"></span></label>
  </div>
  <div class="accessory">
    <div><div class="accName">Brosse gauche</div><div class="accState">Nettoyage bordure</div></div>
    <label class="switch"><input type="checkbox" )rawliteral" + checked(gaucheState) + R"rawliteral( onchange="toggle('/gauche_acc_', this.checked)"><span class="slider"></span></label>
  </div>
  <div class="accessory">
    <div><div class="accName">Brosse droite</div><div class="accState">Nettoyage bordure</div></div>
    <label class="switch"><input type="checkbox" )rawliteral" + checked(droiteState) + R"rawliteral( onchange="toggle('/droite_acc_', this.checked)"><span class="slider"></span></label>
  </div>
</div>

</div>
<script>
function cmd(url){
  fetch(url).then(()=>{ refreshStatus(); });
}
function toggle(base,state){
  fetch(base+(state?'on':'off')).then(()=>{ location.reload(); });
}
function setActive(mode){
  document.getElementById("btnManuel").classList.remove("active");
  document.getElementById("btnAuto").classList.remove("active");
  document.getElementById("btnRetour").classList.remove("active");
  if(mode=="MANUEL"){
    document.getElementById("modeText").innerText="Manuel";
    document.getElementById("btnManuel").classList.add("active");
  } else if(mode=="AUTO"){
    document.getElementById("modeText").innerText="Automatique";
    document.getElementById("btnAuto").classList.add("active");
  } else if(mode=="RETOUR_BASE"){
    document.getElementById("modeText").innerText="Retour base";
    document.getElementById("btnRetour").classList.add("active");
  }
}
function refreshStatus(){
  fetch('/status').then(r=>r.text()).then(mode=>{ setActive(mode); });
}
function refreshDebug(){
  fetch('/debug').then(r=>r.json()).then(d=>{
    let el = document.getElementById("dbSignal");
    el.innerText = d.signal;
    el.className = "debugVal " + d.signal.toLowerCase();
    document.getElementById("dbTemps").innerText = d.tempsSinceMs + " ms";
    document.getElementById("dbMode").innerText = d.mode;
  });
}
function refreshRaw(){
  fetch('/debugraw').then(r=>r.json()).then(d=>{
    document.getElementById("dbRawlen").innerText = d.rawlen;
    document.getElementById("dbBursts").innerText = d.nbBursts;
    document.getElementById("dbT1").innerText     = d.t1;
    document.getElementById("dbT2").innerText     = d.t2;
    document.getElementById("dbT3").innerText     = d.t3;
    document.getElementById("dbTotal").innerText  = d.total;
  });
}
setInterval(refreshStatus, 1000);
setInterval(refreshDebug,  500);
setInterval(refreshRaw,    500);
</script>
</body>
</html>
)rawliteral";

  return html;
}

void initWebControl() {
  WiFi.softAP(apName, apPassword);

  server.on("/", []() {
    server.send(200, "text/html", pageHTML());
  });

  server.on("/status", []() {
    if (modeRobot == MODE_AUTO)             server.send(200, "text/plain", "AUTO");
    else if (modeRobot == MODE_RETOUR_BASE) server.send(200, "text/plain", "RETOUR_BASE");
    else                                    server.send(200, "text/plain", "MANUEL");
  });

  server.on("/debug", []() {
    String modeStr = "MANUEL";
    if (modeRobot == MODE_AUTO)             modeStr = "AUTO";
    else if (modeRobot == MODE_RETOUR_BASE) modeStr = "RETOUR_BASE";

    String json = "{";
    json += "\"signal\":\"" + signalTexte() + "\",";
    json += "\"tempsSinceMs\":" + String(millis() - dernierSignalStationMs) + ",";
    json += "\"mode\":\"" + modeStr + "\"";
    json += "}";
    server.send(200, "application/json", json);
  });

  server.on("/debugraw", []() {
    IRRawDebug d = getIRRawDebug();

    String json = "{";
    json += "\"rawlen\":"   + String(d.rawlen)   + ",";
    json += "\"t1\":"       + String(d.t1)        + ",";
    json += "\"t2\":"       + String(d.t2)        + ",";
    json += "\"t3\":"       + String(d.t3)        + ",";
    json += "\"nbBursts\":" + String(d.nbBursts)  + ",";
    json += "\"total\":"    + String(d.total);
    json += "}";
    server.send(200, "application/json", json);
  });

  server.on("/mode_manuel", []() {
    modeRobot = MODE_MANUEL;
    moveCmd = CMD_STOP;
    stopRoues();
    accessoiresOff();
    server.send(200, "text/plain", "manual");
  });

  server.on("/mode_auto", []() {
    stopRoues();
    accessoiresOn();
    modeRobot = MODE_AUTO;
    server.send(200, "text/plain", "auto");
  });

  server.on("/retour_base", []() {
    stopRoues();
    accessoiresOff();
    modeRobot = MODE_RETOUR_BASE;
    server.send(200, "text/plain", "retour base");
  });

  server.on("/avance", []() {
    modeRobot = MODE_MANUEL;
    moveCmd = CMD_AVANCE;
    appliquerMouvement();
    server.send(200, "text/plain", "forward");
  });

  server.on("/recule", []() {
    modeRobot = MODE_MANUEL;
    moveCmd = CMD_RECULE;
    appliquerMouvement();
    server.send(200, "text/plain", "backward");
  });

  server.on("/gauche", []() {
    modeRobot = MODE_MANUEL;
    moveCmd = CMD_GAUCHE;
    appliquerMouvement();
    server.send(200, "text/plain", "left");
  });

  server.on("/droite", []() {
    modeRobot = MODE_MANUEL;
    moveCmd = CMD_DROITE;
    appliquerMouvement();
    server.send(200, "text/plain", "right");
  });

  server.on("/stop", []() {
    modeRobot = MODE_MANUEL;
    moveCmd = CMD_STOP;
    stopRoues();
    server.send(200, "text/plain", "stop");
  });

  server.on("/vac_on",         []() { vacState = true;     appliquerAccessoires(); server.send(200, "text/plain", "vac on"); });
  server.on("/vac_off",        []() { vacState = false;    appliquerAccessoires(); server.send(200, "text/plain", "vac off"); });
  server.on("/main_on",        []() { mainState = true;    appliquerAccessoires(); server.send(200, "text/plain", "main on"); });
  server.on("/main_off",       []() { mainState = false;   appliquerAccessoires(); server.send(200, "text/plain", "main off"); });
  server.on("/gauche_acc_on",  []() { gaucheState = true;  appliquerAccessoires(); server.send(200, "text/plain", "left on"); });
  server.on("/gauche_acc_off", []() { gaucheState = false; appliquerAccessoires(); server.send(200, "text/plain", "left off"); });
  server.on("/droite_acc_on",  []() { droiteState = true;  appliquerAccessoires(); server.send(200, "text/plain", "right on"); });
  server.on("/droite_acc_off", []() { droiteState = false; appliquerAccessoires(); server.send(200, "text/plain", "right off"); });

  server.begin();
}

void updateWebControl() {
  server.handleClient();
}