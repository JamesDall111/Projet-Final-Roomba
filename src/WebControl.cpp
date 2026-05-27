#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h> 

#include "WebControl.h"
#include "Moteur.h"
#include "Accessoires.h"
#include "Batterie.h"
#include "Capteur_IR.h"
#include "Gestion_SansFil.h" 

const char* apName     = "SMD-Control";
const char* apPassword = "12345678";

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81); 

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

const String stylesPartages = R"rawliteral(
*{box-sizing:border-box}
body{margin:0;font-family:Arial;background:linear-gradient(135deg,#eef3ff,#dbe4ff);color:#0f172a}
.app{max-width:430px;margin:auto;min-height:100vh;padding:18px}
.header{text-align:center;margin-bottom:12px}
.logo{font-size:28px;font-weight:900}
.logo span{color:#2563eb}
.nav-tabs{display:flex;gap:8px;margin-bottom:18px;background:rgba(255,255,255,.5);padding:6px;border-radius:16px}
.nav-link{flex:1;text-align:center;padding:10px;text-decoration:none;color:#64748b;font-weight:700;font-size:14px;border-radius:12px;transition:.2s}
.nav-link.active{background:white;color:#2563eb;box-shadow:0 4px 12px #64748b15}
.card{background:rgba(255,255,255,.88);border-radius:28px;padding:20px;margin-bottom:16px;box-shadow:0 20px 45px #64748b33}
.robot{text-align:center}
.robotCircle{width:160px;height:160px;margin:10px auto;border-radius:50%;background:linear-gradient(145deg,#fff,#dbeafe);box-shadow:inset 0 0 0 12px #f8fafc,0 18px 35px #94a3b844;position:relative}
.robotCircle:before{content:"";position:absolute;width:46px;height:46px;background:#e2e8f0;border-radius:50%;top:50px;left:57px;box-shadow:inset 0 0 0 8px white}
.robotCircle:after{content:"";position:absolute;width:60px;height:8px;background:#0f172a;border-radius:20px;bottom:28px;left:50px}
.battery{font-size:36px;font-weight:900;color:#2563eb}
.batteryText{color:#64748b;font-size:14px}
.rssiText{color:#64748b;font-size:14px;margin-top:5px;font-weight:bold;}
.bar{width:100%;height:14px;background:#e2e8f0;border-radius:20px;overflow:hidden;margin-top:12px}
.modeGrid{display:grid;grid-template-columns:1fr 1fr;gap:12px}
.modeBtn{padding:18px;border:0;border-radius:22px;background:#f8fafc;font-size:16px;font-weight:800;color:#64748b;box-shadow:inset 0 0 0 1px #e2e2f0;cursor:pointer}
.modeBtn.active{background:linear-gradient(135deg,#2563eb,#1d4ed8);color:white;box-shadow:0 12px 24px #2563eb55}
.modeBtn.big{grid-column:span 2}
.controlGrid{display:grid;grid-template-columns:1fr 1fr;gap:12px}
.btn{border:0;border-radius:22px;padding:18px;font-size:16px;font-weight:800;color:white;background:#2563eb;box-shadow:0 12px 24px #2563eb44;cursor:pointer}
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
.slider{position:absolute;inset:0;background:#cbd5e1;border-radius:999px;transition:.2s;cursor:pointer}
.slider:before{content:"";position:absolute;width:28px;height:28px;left:3px;top:3px;background:white;border-radius:50%;transition:.2s;box-shadow:0 4px 10px #0003}
input:checked + .slider{background:#2563eb}
input:checked + .slider:before{transform:translateX(30px)}
.debug{background:#0f172a;border-radius:28px;padding:20px;margin-bottom:16px}
.debug h2{color:white;margin:0 0 14px}
.debugRow{display:flex;justify-content:space-between;align-items:center;padding:10px 0;border-bottom:1px solid #1e293b}
.debugRow:last-child{border-bottom:0}
.debugLabel{color:#94a3b8;font-size:14px}
.debugVal{font-weight:900;font-size:15px;color:white}
.debugVal.vu, .debugVal.connecte{color:#22c55e}
.debugVal.aucun, .debugVal.deconnecte{color:#ef4444}
.rawBlock{margin-top:12px;padding:12px;background:#1e293b;border-radius:16px}
.rawTitle{color:#94a3b8;font-size:12px;margin-bottom:8px;font-weight:700;text-transform:uppercase}
.rawGrid{display:grid;grid-template-columns:1fr 1fr;gap:8px}
.rawItem{background:#0f172a;border-radius:10px;padding:8px 10px}
.rawItemLabel{color:#475569;font-size:11px}
.rawItemVal{color:white;font-weight:900;font-size:15px}
#ws-toast{font-size:11px;color:#ef4444;text-align:center;margin-top:-10px;margin-bottom:10px;font-weight:bold;}
)rawliteral";

String pageHTML() {
  float tension = lireTensionBatterie();
  int pct = lirePourcentageBatterie(tension);

  String autoActive   = (modeRobot == MODE_AUTO)           ? "active" : "";
  String manualActive = (modeRobot == MODE_MANUEL)          ? "active" : "";
  String retourActive = (modeRobot == MODE_RETOUR_BASE) ? "active" : "";

  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>SuckMyDust - Controle</title>
<style>)rawliteral" + stylesPartages + R"rawliteral(</style>
</head>
<body>
<div class="app">

<div class="header">
  <div class="logo">Suck<span>My</span>Dust</div>
  <div id="ws-toast">Connexion réseau...</div>
</div>

<div class="nav-tabs">
  <a href="/" class="nav-link active">Controle</a>
  <a href="/debug-page" class="nav-link">Debug Diagnostic</a>
</div>

<div class="card robot">
  <div class="robotCircle"></div>
  <div class="battery" id="txtPct">)rawliteral" + String(pct) + R"rawliteral(%</div>
  <div class="batteryText" id="txtVolts">Batterie - )rawliteral" + String(tension, 2) + R"rawliteral( V</div>
  
  <div class="rssiText">Signal Station : <span id="txtRssi" style="color:#22c55e;">-- dBm</span></div>
  
  <div class="bar"><div id="barFill" class="fill" style="height:100%;background:linear-gradient(90deg,#22c55e,#2563eb);width:)rawliteral" + String(pct) + R"rawliteral(%;"></div></div>
</div>

<div class="card">
  <h2>Mode de nettoyage</h2>
  <div class="modeStatus">Mode actuel : <span id="modeText">)rawliteral" + modeTexte() + R"rawliteral(</span></div>
  <div class="modeGrid">
    <button id="btnManuel" class="modeBtn )rawliteral" + manualActive + R"rawliteral(" onclick="sendWS('SET_MODE:MANUEL')">Manuel</button>
    <button id="btnAuto"   class="modeBtn )rawliteral" + autoActive   + R"rawliteral(" onclick="sendWS('SET_MODE:AUTO')">Automatique</button>
    <button id="btnRetour" class="modeBtn big )rawliteral" + retourActive + R"rawliteral(" onclick="sendWS('SET_MODE:RETOUR_BASE')">Retour base</button>
  </div>
</div>

<div class="card">
  <h2>Mouvements</h2>
  <div class="controlGrid">
    <button class="btn big"      onclick="sendWS('MOVE:AVANCE')">Avancer</button>
    <button class="btn"          onclick="sendWS('MOVE:GAUCHE')">Gauche</button>
    <button class="btn"          onclick="sendWS('MOVE:DROITE')">Droite</button>
    <button class="btn big"      onclick="sendWS('MOVE:RECULE')">Reculer</button>
    <button class="btn stop big" onclick="sendWS('MOVE:STOP')">Arret</button>
  </div>
</div>

<div class="card">
  <h2>Accessoires</h2>
  <div class="accessory">
    <div><div class="accName">Aspirateur</div><div class="accState">Moteur aspiration</div></div>
    <label class="switch"><input type="checkbox" id="chkVac" )rawliteral" + checked(vacState) + R"rawliteral( onchange="sendWS('TOGGLE:VAC:'+this.checked)"><span class="slider"></span></label>
  </div>
  <div class="accessory">
    <div><div class="accName">Brosse centrale</div><div class="accState">Nettoyage principal</div></div>
    <label class="switch"><input type="checkbox" id="chkMain" )rawliteral" + checked(mainState) + R"rawliteral( onchange="sendWS('TOGGLE:MAIN:'+this.checked)"><span class="slider"></span></label>
  </div>
  <div class="accessory">
    <div><div class="accName">Brosse gauche</div><div class="accState">Nettoyage bordure</div></div>
    <label class="switch"><input type="checkbox" id="chkG" )rawliteral" + checked(gaucheState) + R"rawliteral( onchange="sendWS('TOGGLE:GAUCHE:'+this.checked)"><span class="slider"></span></label>
  </div>
  <div class="accessory">
    <div><div class="accName">Brosse droite</div><div class="accState">Nettoyage bordure</div></div>
    <label class="switch"><input type="checkbox" id="chkD" )rawliteral" + checked(droiteState) + R"rawliteral( onchange="sendWS('TOGGLE:DROITE:'+this.checked)"><span class="slider"></span></label>
  </div>
</div>

</div>
<script>
var ws;
function initWS(){
  ws = new WebSocket('ws://'+window.location.hostname+':81/');
  ws.onopen = () => { 
    document.getElementById('ws-toast').innerText = "CONNECTÉ EN TEMPS RÉEL"; 
    document.getElementById('ws-toast').style.color = "#22c55e";
  };
  ws.onclose = () => { 
    document.getElementById('ws-toast').innerText = "DÉCONNECTÉ - RETENTATIVE..."; 
    document.getElementById('ws-toast').style.color = "#ef4444";
    setTimeout(initWS, 1500); 
  };
  ws.onmessage = (e) => {
    let d = JSON.parse(e.data);
    if(d.type === "TELEMETRIE") {
      document.getElementById("txtPct").innerText = d.pct + "%";
      document.getElementById("txtVolts").innerText = "Batterie - " + d.tension.toFixed(2) + " V";
      document.getElementById("barFill").style.width = d.pct + "%";
      
      // Mise à jour du RSSI avec couleur dynamique selon la force
      let rssiEl = document.getElementById("txtRssi");
      rssiEl.innerText = d.rssi + " dBm";
      if(d.rssi >= -65) rssiEl.style.color = "#22c55e";      // Excellent (Vert)
      else if(d.rssi >= -82) rssiEl.style.color = "#f59e0b"; // Moyen (Orange)
      else rssiEl.style.color = "#ef4444";                   // Critique (Rouge)
      
      document.getElementById("btnManuel").classList.toggle("active", d.mode === "MANUEL");
      document.getElementById("btnAuto").classList.toggle("active", d.mode === "AUTO");
      document.getElementById("btnRetour").classList.toggle("active", d.mode === "RETOUR_BASE");
      
      let txt = "Manuel";
      if(d.mode === "AUTO") txt = "Automatique";
      if(d.mode === "RETOUR_BASE") txt = "Retour base";
      document.getElementById("modeText").innerText = txt;

      document.getElementById("chkVac").checked = d.vac;
      document.getElementById("chkMain").checked = d.main;
      document.getElementById("chkG").checked = d.gauche;
      document.getElementById("chkD").checked = d.droite;
    }
  };
}
function sendWS(msg){ if(ws && ws.readyState===1) ws.send(msg); }
window.onload = initWS;
</script>
</body>
</html>
)rawliteral";
  return html;
}

String pageDebugHTML() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>SuckMyDust - Diagnostic</title>
<style>)rawliteral" + stylesPartages + R"rawliteral(</style>
</head>
<body>
<div class="app">

<div class="header">
  <div class="logo">Suck<span>My</span>Dust</div>
  <div id="ws-toast">Connexion diagnostic...</div>
</div>

<div class="nav-tabs">
  <a href="/" class="nav-link">Controle</a>
  <a href="/debug-page" class="nav-link active">Debug Diagnostic</a>
</div>

<div class="debug" style="background: linear-gradient(135deg, #1e1b4b, #311042); margin-bottom: 16px;">
  <h2 style="color: #60a5fa;">Liaison Station (ESP-NOW)</h2>
  <div class="debugRow">
    <div class="debugLabel" style="color: #93c5fd;">Amarrage Physique</div>
    <div class="debugVal deconnecte" id="dbNowContact">--</div>
  </div>
  <div class="debugRow">
    <div class="debugLabel" style="color: #93c5fd;">Ordre de la Station</div>
    <div class="debugVal" id="dbNowMsg" style="color: #cbd5e1;">--</div>
  </div>
  <div class="debugRow">
    <div class="debugLabel" style="color: #93c5fd;">Force Signal Radio</div>
    <div class="debugVal" id="dbNowRssi" style="color: #cbd5e1;">-- dBm</div>
  </div>
</div>

<div class="debug">
  <h2>Debug Logique IR</h2>
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
    <div class="rawTitle">Valeurs brutes recepteur</div>
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

</div>
<script>
var ws;
function initWS(){
  ws = new WebSocket('ws://'+window.location.hostname+':81/');
  ws.onopen = () => { 
    document.getElementById('ws-toast').innerText = "DIAGNOSTIC ACTIF (LIVE)"; 
    document.getElementById('ws-toast').style.color = "#22c55e";
  };
  ws.onclose = () => { 
    document.getElementById('ws-toast').innerText = "DÉCONNECTÉ"; 
    document.getElementById('ws-toast').style.color = "#ef4444";
    setTimeout(initWS, 1500); 
  };
  ws.onmessage = (e) => {
    let d = JSON.parse(e.data);
    if(d.type === "DEBUG") {
      let el = document.getElementById("dbSignal");
      el.innerText = d.signal;
      el.className = "debugVal " + d.signal.toLowerCase();
      document.getElementById("dbTemps").innerText = d.tempsSinceMs + " ms";
      document.getElementById("dbMode").innerText = d.mode;
      document.getElementById("dbNowRssi").innerText = d.rssi + " dBm";
      
      let elContact = document.getElementById("dbNowContact");
      if(d.nowContact) {
        elContact.innerText = "CONNECTE (Plots)";
        elContact.className = "debugVal connecte";
      } else {
        elContact.innerText = "HORS LIGNE";
        elContact.className = "debugVal deconnecte";
      }
      document.getElementById("dbNowMsg").innerText = d.nowMsg;
      
      document.getElementById("dbRawlen").innerText = d.rawlen;
      document.getElementById("dbBursts").innerText = d.nbBursts;
      document.getElementById("dbT1").innerText     = d.t1;
      document.getElementById("dbT2").innerText     = d.t2;
      document.getElementById("dbT3").innerText     = d.t3;
      document.getElementById("dbTotal").innerText  = d.total;
    }
  };
}
window.onload = initWS;
</script>
</body>
</html>
)rawliteral";
  return html;
}

// --- TRANSMISSION DU RSSI VIA JSON ---
void diffuserDonnees() {
  String modeStr = "MANUEL";
  if (modeRobot == MODE_AUTO)        modeStr = "AUTO";
  if (modeRobot == MODE_RETOUR_BASE) modeStr = "RETOUR_BASE";

  float tension = lireTensionBatterie();
  int pct = lirePourcentageBatterie(tension);

  // 1. Envoi JSON pour la page principale Controle (avec rssiStation)
  String jsonTelemetrie = "{\"type\":\"TELEMETRIE\",";
  jsonTelemetrie += "\"mode\":\"" + modeStr + "\",";
  jsonTelemetrie += "\"tension\":" + String(tension, 2) + ",";
  jsonTelemetrie += "\"pct\":" + String(pct) + ",";
  jsonTelemetrie += "\"rssi\":" + String(rssiStation) + ","; // Injection de la variable globale
  jsonTelemetrie += "\"vac\":" + String(vacState ? "true" : "false") + ",";
  jsonTelemetrie += "\"main\":" + String(mainState ? "true" : "false") + ",";
  jsonTelemetrie += "\"gauche\":" + String(gaucheState ? "true" : "false") + ",";
  jsonTelemetrie += "\"droite\":" + String(droiteState ? "true" : "false");
  jsonTelemetrie += "}";
  webSocket.broadcastTXT(jsonTelemetrie);

  // 2. Envoi JSON pour la page de Diagnostic Debug (avec rssiStation)
  IRRawDebug d = getIRRawDebug();
  String jsonDebug = "{\"type\":\"DEBUG\",";
  jsonDebug += "\"signal\":\"" + signalTexte() + "\",";
  jsonDebug += "\"tempsSinceMs\":" + String(millis() - dernierSignalStationMs) + ",";
  jsonDebug += "\"mode\":\"" + modeStr + "\",";
  jsonDebug += "\"rssi\":" + String(rssiStation) + ","; // Injection de la variable globale
  jsonDebug += "\"nowContact\":" + String(donneesStation.contactsAlimentes ? "true" : "false") + ",";
  jsonDebug += "\"nowMsg\":\"" + String(donneesStation.message) + "\",";
  jsonDebug += "\"rawlen\":" + String(d.rawlen) + ",";
  jsonDebug += "\"t1\":" + String(d.t1) + ",";
  jsonDebug += "\"t2\":" + String(d.t2) + ",";
  jsonDebug += "\"t3\":" + String(d.t3) + ",";
  jsonDebug += "\"nbBursts\":" + String(d.nbBursts) + ",";
  jsonDebug += "\"total\":" + String(d.total);
  jsonDebug += "}";
  webSocket.broadcastTXT(jsonDebug);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    String msg = String((char*)payload);

    if (msg.startsWith("SET_MODE:")) {
      String modeTarget = msg.substring(9);
      stopRoues();
      if (modeTarget == "MANUEL") { modeRobot = MODE_MANUEL; accessoiresOff(); moveCmd = CMD_STOP; }
      else if (modeTarget == "AUTO") { modeRobot = MODE_AUTO; accessoiresOn(); }
      else if (modeTarget == "RETOUR_BASE") { modeRobot = MODE_RETOUR_BASE; accessoiresOff(); }
      diffuserDonnees();
    }
    else if (msg.startsWith("MOVE:")) {
      String m = msg.substring(5);
      modeRobot = MODE_MANUEL;
      if (m == "AVANCE") moveCmd = CMD_AVANCE;
      else if (m == "RECULE") moveCmd = CMD_RECULE;
      else if (m == "GAUCHE") moveCmd = CMD_GAUCHE;
      else if (m == "DROITE") moveCmd = CMD_DROITE;
      else { moveCmd = CMD_STOP; stopRoues(); accessoiresOff(); }
      appliquerMouvement();
      diffuserDonnees();
    }
    else if (msg.startsWith("TOGGLE:")) {
      String sub = msg.substring(7);
      if (sub.startsWith("VAC:")) vacState = (sub.substring(4) == "true");
      else if (sub.startsWith("MAIN:")) mainState = (sub.substring(5) == "true");
      else if (sub.startsWith("GAUCHE:")) gaucheState = (sub.substring(7) == "true");
      else if (sub.startsWith("DROITE:")) droiteState = (sub.substring(7) == "true");
      appliquerAccessoires();
      diffuserDonnees();
    }
  }
}

void initWebControl() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(apName, apPassword);

  server.on("/", []() { server.send(200, "text/html", pageHTML()); });
  server.on("/debug-page", []() { server.send(200, "text/html", pageDebugHTML()); });

  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void updateWebControl() {
  server.handleClient();
  webSocket.loop();

  static unsigned long chronoDiff = 0;
  if (millis() - chronoDiff >= 400) {
    chronoDiff = millis();
    diffuserDonnees();
  }
}