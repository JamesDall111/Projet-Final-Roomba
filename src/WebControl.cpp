#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "WebControl.h"
#include "Moteur.h"
#include "Accessoires.h"
#include "Batterie.h"

const char* apName = "SMD-Control";
const char* apPassword = "12345678";

WebServer server(80);

ModeRobot modeRobot = MODE_MANUEL;

String checked(bool state) {
  return state ? "checked" : "";
}

String pageHTML() {
  float tension = lireTensionBatterie();
  int pct = lirePourcentageBatterie(tension);

  String autoActive = (modeRobot == MODE_AUTO) ? "active" : "";
  String manualActive = (modeRobot == MODE_MANUEL) ? "active" : "";

  String html = R"rawliteral(
<!DOCTYPE html>
<html>s
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>SuckMyDust</title>

<style>
*{box-sizing:border-box}
body{
  margin:0;
  font-family:Arial, Helvetica, sans-serif;
  background:linear-gradient(135deg,#eef3ff,#dbe4ff);
  color:#0f172a;
}
.app{
  max-width:430px;
  margin:auto;
  min-height:100vh;
  padding:18px;
}
.header{
  display:flex;
  justify-content:center;
  align-items:center;
  margin-bottom:18px;
}
.logo{
  font-size:28px;
  font-weight:900;
}
.logo span{color:#2563eb}
.card{
  background:rgba(255,255,255,.85);
  border-radius:28px;
  padding:20px;
  margin-bottom:16px;
  box-shadow:0 20px 45px #64748b33;
}
.robot{
  text-align:center;
}
.robotCircle{
  width:190px;
  height:190px;
  margin:10px auto;
  border-radius:50%;
  background:linear-gradient(145deg,#ffffff,#dbeafe);
  box-shadow:inset 0 0 0 12px #f8fafc, 0 18px 35px #94a3b844;
  position:relative;
}
.robotCircle:before{
  content:"";
  position:absolute;
  width:52px;
  height:52px;
  background:#e2e8f0;
  border-radius:50%;
  top:58px;
  left:69px;
  box-shadow:inset 0 0 0 8px white;
}
.robotCircle:after{
  content:"";
  position:absolute;
  width:70px;
  height:10px;
  background:#0f172a;
  border-radius:20px;
  bottom:32px;
  left:60px;
}
.battery{
  font-size:36px;
  font-weight:900;
  color:#2563eb;
}
.batteryText{
  color:#64748b;
  font-size:14px;
}
.bar{
  width:100%;
  height:14px;
  background:#e2e8f0;
  border-radius:20px;
  overflow:hidden;
  margin-top:12px;
}
.fill{
  height:100%;
  background:linear-gradient(90deg,#22c55e,#2563eb);
  width:)rawliteral" + String(pct) + R"rawliteral(%;
}
.modeGrid{
  display:grid;
  grid-template-columns:1fr 1fr;
  gap:12px;
}
.modeBtn{
  padding:18px;
  border:0;
  border-radius:22px;
  background:#f8fafc;
  font-size:16px;
  font-weight:800;
  color:#64748b;
  box-shadow:inset 0 0 0 1px #e2e8f0;
}
.modeBtn.active{
  background:linear-gradient(135deg,#2563eb,#1d4ed8);
  color:white;
  box-shadow:0 12px 24px #2563eb55;
}
.controlGrid{
  display:grid;
  grid-template-columns:1fr 1fr;
  gap:12px;
}
.btn{
  border:0;
  border-radius:22px;
  padding:18px;
  font-size:16px;
  font-weight:800;
  color:white;
  background:#2563eb;
  box-shadow:0 12px 24px #2563eb44;
}
.btn.big{grid-column:span 2}
.btn.stop{
  background:#ef4444;
  box-shadow:0 12px 24px #ef444444;
}
h2{
  margin:0 0 14px;
  font-size:20px;
}
.accessory{
  display:flex;
  justify-content:space-between;
  align-items:center;
  padding:14px 0;
  border-bottom:1px solid #e2e8f0;
}
.accessory:last-child{border-bottom:0}
.accName{
  font-weight:800;
}
.accState{
  font-size:13px;
  color:#64748b;
}
.switch{
  position:relative;
  width:64px;
  height:34px;
}
.switch input{display:none}
.slider{
  position:absolute;
  inset:0;
  background:#cbd5e1;
  border-radius:999px;
  transition:.2s;
}
.slider:before{
  content:"";
  position:absolute;
  width:28px;
  height:28px;
  left:3px;
  top:3px;
  background:white;
  border-radius:50%;
  transition:.2s;
  box-shadow:0 4px 10px #0003;
}
input:checked + .slider{
  background:#2563eb;
}
input:checked + .slider:before{
  transform:translateX(30px);
}
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

  <div class="card">
    <h2>Mode de nettoyage</h2>
    <div class="modeGrid">
      <button class="modeBtn )rawliteral" + manualActive + R"rawliteral(" onclick="cmd('/mode_manuel')">Manuel</button>
      <button class="modeBtn )rawliteral" + autoActive + R"rawliteral(" onclick="cmd('/mode_auto')">Automatique</button>
    </div>
  </div>

  <div class="card">
    <h2>Deplacement</h2>
    <div class="controlGrid">
      <button class="btn big" onclick="cmd('/avance')">Avancer</button>
      <button class="btn" onclick="cmd('/gauche')">Gauche</button>
      <button class="btn" onclick="cmd('/droite')">Droite</button>
      <button class="btn big" onclick="cmd('/recule')">Reculer</button>
      <button class="btn stop big" onclick="cmd('/stop')">Arret</button>
    </div>
  </div>

  <div class="card">
    <h2>Accessoires</h2>

    <div class="accessory">
      <div>
        <div class="accName">Aspirateur</div>
        <div class="accState">Moteur aspiration</div>
      </div>
      <label class="switch">
        <input type="checkbox" )rawliteral" + checked(vacState) + R"rawliteral( onchange="toggle('/vac_', this.checked)">
        <span class="slider"></span>
      </label>
    </div>

    <div class="accessory">
      <div>
        <div class="accName">Brosse centrale</div>
        <div class="accState">Nettoyage principal</div>
      </div>
      <label class="switch">
        <input type="checkbox" )rawliteral" + checked(mainState) + R"rawliteral( onchange="toggle('/main_', this.checked)">
        <span class="slider"></span>
      </label>
    </div>

    <div class="accessory">
      <div>
        <div class="accName">Brosse gauche</div>
        <div class="accState">Nettoyage bordure</div>
      </div>
      <label class="switch">
        <input type="checkbox" )rawliteral" + checked(gaucheState) + R"rawliteral( onchange="toggle('/gauche_acc_', this.checked)">
        <span class="slider"></span>
      </label>
    </div>

    <div class="accessory">
      <div>
        <div class="accName">Brosse droite</div>
        <div class="accState">Nettoyage bordure</div>
      </div>
      <label class="switch">
        <input type="checkbox" )rawliteral" + checked(droiteState) + R"rawliteral( onchange="toggle('/droite_acc_', this.checked)">
        <span class="slider"></span>
      </label>
    </div>
  </div>

</div>

<script>
function cmd(url){
  fetch(url).then(()=>{
    if(url.includes("mode")){
      location.reload();
    }
  });
}

function toggle(base, state){
  fetch(base + (state ? 'on' : 'off')).then(()=>{
    location.reload();
  });
}
</script>

</body>
</html>
)rawliteral";

  return html;
}

// =====================
// INIT WEB
// =====================
void initWebControl() {
  WiFi.softAP(apName, apPassword);

  server.on("/", []() {
    server.send(200, "text/html", pageHTML());
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

  server.on("/vac_on", []() {
    vacState = true;
    appliquerAccessoires();
    server.send(200, "text/plain", "vac on");
  });

  server.on("/vac_off", []() {
    vacState = false;
    appliquerAccessoires();
    server.send(200, "text/plain", "vac off");
  });

  server.on("/main_on", []() {
    mainState = true;
    appliquerAccessoires();
    server.send(200, "text/plain", "main on");
  });

  server.on("/main_off", []() {
    mainState = false;
    appliquerAccessoires();
    server.send(200, "text/plain", "main off");
  });

  server.on("/gauche_acc_on", []() {
    gaucheState = true;
    appliquerAccessoires();
    server.send(200, "text/plain", "left brush on");
  });

  server.on("/gauche_acc_off", []() {
    gaucheState = false;
    appliquerAccessoires();
    server.send(200, "text/plain", "left brush off");
  });

  server.on("/droite_acc_on", []() {
    droiteState = true;
    appliquerAccessoires();
    server.send(200, "text/plain", "right brush on");
  });

  server.on("/droite_acc_off", []() {
    droiteState = false;
    appliquerAccessoires();
    server.send(200, "text/plain", "right brush off");
  });

  server.begin();
}

// =====================
// UPDATE WEB
// =====================
void updateWebControl() {
  server.handleClient();
}