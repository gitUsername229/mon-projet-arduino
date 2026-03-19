// --- FICHIER : page_web.h ---
#ifndef PAGE_WEB_H
#define PAGE_WEB_H
#include <pgmspace.h>

const char PAGE_LOGIN[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>Accès Sécurisé</title>
  <style>
    * { box-sizing: border-box; }
    body {
      margin: 0; padding: 0; min-height: 100vh;
      font-family: system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
      background: linear-gradient(-45deg, #0b0f19, #1c1c38, #050b14, #122240);
      background-size: 400% 400%; animation: gradientBG 15s ease infinite;
      color: #fff; display: flex; justify-content: center; align-items: center;
      overflow: hidden;
    }
    @keyframes gradientBG { 0% { background-position: 0% 50%; } 50% { background-position: 100% 50%; } 100% { background-position: 0% 50%; } }
    .liquid-glass {
      background: rgba(255, 255, 255, 0.03);
      border: 1px solid rgba(255, 255, 255, 0.08);
      box-shadow: 0 25px 45px rgba(0,0,0,0.5), inset 0 1px 1px rgba(255,255,255,0.1);
      backdrop-filter: blur(25px); -webkit-backdrop-filter: blur(25px);
      border-radius: 28px; padding: 40px 25px; width: 90%; max-width: 380px;
      text-align: center; animation: dropIn 0.8s cubic-bezier(0.2, 0.8, 0.2, 1);
    }
    @keyframes dropIn { from { opacity: 0; transform: translateY(-30px) scale(0.95); } to { opacity: 1; transform: translateY(0) scale(1); } }
    h2 { margin: 0 0 35px; font-weight: 800; font-size: clamp(22px, 6vw, 28px); color: transparent; background: linear-gradient(90deg, #00d2ff, #73a7ff); -webkit-background-clip: text; letter-spacing: 1px; }
    .form-group { margin-bottom: 25px; text-align: left; }
    label { font-size: 11px; font-weight: 700; color: rgba(255,255,255,0.6); margin-bottom: 8px; display: block; text-transform: uppercase; letter-spacing: 1px; }
    input[type='password'] {
      width: 100%; padding: 18px 20px; border: none; border-radius: 16px;
      background: rgba(0, 0, 0, 0.3); color: #fff; font-size: 20px; outline: none;
      border: 1px solid rgba(255,255,255,0.05); letter-spacing: 4px; font-family: monospace;
      transition: all 0.3s ease; box-shadow: inset 0 2px 5px rgba(0,0,0,0.5);
    }
    input[type='password']:focus { border-color: rgba(0, 210, 255, 0.5); background: rgba(0,0,0,0.5); box-shadow: 0 0 20px rgba(0,210,255,0.2), inset 0 2px 5px rgba(0,0,0,0.7); }
    button {
      width: 100%; padding: 18px; border: none; border-radius: 16px;
      background: linear-gradient(135deg, #00d2ff 0%, #3a7bd5 100%);
      color: white; font-weight: 800; font-size: 15px; cursor: pointer; transition: all 0.3s cubic-bezier(0.2, 0.8, 0.2, 1);
      text-transform: uppercase; letter-spacing: 2px; box-shadow: 0 8px 20px rgba(58, 123, 213, 0.4);
    }
    button:hover { transform: translateY(-4px); box-shadow: 0 12px 25px rgba(58, 123, 213, 0.6); filter: brightness(1.1); }
    button:active { transform: translateY(2px); box-shadow: 0 4px 10px rgba(58, 123, 213, 0.3); }
    .error { font-size: 13px; font-weight: 600; color: #ff6b81; background: rgba(255,107,129,0.1); padding: 12px; border-radius: 12px; border: 1px solid rgba(255,107,129,0.3); margin-bottom: 25px; animation: shake 0.4s; }
    @keyframes shake { 0%, 100% { transform: translateX(0); } 25% { transform: translateX(-5px); } 75% { transform: translateX(5px); } }
  </style>
</head>
<body>
  <div class="liquid-glass">
    <h2>SÉCURITÉ CASIER</h2>
    %ERROR_MSG%
    <form action="/login" method="POST">
      <div class="form-group">
        <label>Mot de passe Web</label>
        <input type="password" name="password" placeholder="••••••••" required>
      </div>
      <button type="submit">Déverrouiller</button>
    </form>
  </div>
</body>
</html>
)=====";

const char PAGE_DASHBOARD[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>Dashboard Casier</title>
  <style>
    * { box-sizing: border-box; }
    body {
      margin: 0; padding: 0; min-height: 100vh;
      font-family: system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
      background: linear-gradient(-45deg, #0b0f19, #1c1c38, #050b14, #122240);
      background-size: 400% 400%; animation: gradientBG 20s ease infinite;
      color: #fff; padding-bottom: 40px;
    }
    @keyframes gradientBG { 0% { background-position: 0% 50%; } 50% { background-position: 100% 50%; } 100% { background-position: 0% 50%; } }
    
    .liquid-glass {
      background: rgba(255, 255, 255, 0.03); border: 1px solid rgba(255, 255, 255, 0.06);
      box-shadow: 0 15px 35px rgba(0,0,0,0.3), inset 0 1px 1px rgba(255,255,255,0.08);
      backdrop-filter: blur(25px); -webkit-backdrop-filter: blur(25px);
    }
    
    .header { padding: 25px 20px; text-align: center; position: sticky; top: 0; z-index: 100; display: flex; justify-content: space-between; align-items: center; border-radius: 0 0 24px 24px; }
    h1 { margin: 0; font-weight: 800; font-size: clamp(16px, 4vw, 22px); color: transparent; background: linear-gradient(90deg, #00d2ff, #73a7ff); -webkit-background-clip: text; letter-spacing: 2px; text-transform: uppercase; }
    
    .logout { text-decoration: none; color: #ff6b81; font-size: 11px; font-weight: 800; letter-spacing: 1px; text-transform: uppercase; padding: 10px 14px; background: rgba(255,107,129,0.1); border: 1px solid rgba(255,107,129,0.3); border-radius: 12px; transition: all 0.3s; }
    .logout:hover { background: #ff6b81; color: #fff; box-shadow: 0 0 15px rgba(255,107,129,0.5); }
    
    .container { max-width: 900px; margin: 30px auto; padding: 0 15px; }
    
    .tabs { display: flex; justify-content: center; gap: 8px; margin-bottom: 30px; flex-wrap: wrap; background: rgba(0,0,0,0.2); padding: 8px; border-radius: 20px; border: 1px solid rgba(255,255,255,0.05); }
    .tab-btn {
      flex: 1 1 auto; min-width: 80px; text-align: center;
      background: transparent; border: none; color: #aaa; padding: 12px 10px; border-radius: 14px; cursor: pointer;
      font-weight: 700; transition: all 0.3s; text-transform: uppercase; letter-spacing: 1px; font-size: clamp(9px, 2.5vw, 11px);
    }
    .tab-btn:hover { color: #fff; background: rgba(255,255,255,0.05); }
    .tab-btn.active { background: linear-gradient(135deg, #00d2ff 0%, #3a7bd5 100%); color: white; box-shadow: 0 5px 15px rgba(58,123,213,0.4); }
    
    .panel { display: none; border-radius: 24px; padding: clamp(20px, 5vw, 35px); animation: slideUp 0.5s cubic-bezier(0.2, 0.8, 0.2, 1); }
    .panel.active { display: block; }
    @keyframes slideUp { from { opacity: 0; transform: translateY(20px); } to { opacity: 1; transform: translateY(0); } }
    
    h2 { margin: 0 0 25px 0; font-size: clamp(14px, 4vw, 18px); font-weight: 700; color: #fff; display: flex; align-items: center; text-transform: uppercase; letter-spacing: 1px; }
    h2::before { content: ''; display: inline-block; width: 4px; height: 18px; background: #00d2ff; margin-right: 10px; border-radius: 4px; }
    
    table { width: 100%; border-collapse: collapse; border-spacing: 0; }
    th { padding: 15px; text-align: left; font-size: 11px; text-transform: uppercase; color: rgba(255,255,255,0.5); font-weight: 800; letter-spacing: 1px; border-bottom: 1px solid rgba(255,255,255,0.05); }
    td { padding: 15px; text-align: left; font-size: 14px; border-bottom: 1px solid rgba(255,255,255,0.03); color: #ddd; }
    tr:last-child td { border-bottom: none; }
    tr:hover td { background: rgba(255,255,255,0.02); color: #fff; }
    
    .uid-txt { font-family: monospace; font-size: clamp(12px, 3vw, 15px); letter-spacing: 1px; color: #00d2ff; }
    .succes { color: #00e676; font-weight: 700; font-size: 12px; text-transform: uppercase; letter-spacing: 1px; }
    .echec { color: #ff6b81; font-weight: 700; font-size: 12px; text-transform: uppercase; letter-spacing: 1px; }
    
    .btn { padding: 10px 16px; text-decoration: none; border-radius: 10px; font-size: 11px; font-weight: 800; transition: all 0.3s; display: inline-flex; align-items: center; justify-content: center; text-transform: uppercase; letter-spacing: 1px; }
    .btn-add { background: rgba(0,230,118,0.1); border: 1px solid rgba(0,230,118,0.4); color: #00e676; }
    .btn-add:hover { background: #00e676; color: #000; box-shadow: 0 5px 15px rgba(0,230,118,0.4); transform: translateY(-2px); }
    .btn-del { background: rgba(255,107,129,0.1); border: 1px solid rgba(255,107,129,0.4); color: #ff6b81; }
    .btn-del:hover { background: #ff6b81; color: #fff; box-shadow: 0 5px 15px rgba(255,107,129,0.4); transform: translateY(-2px); }
    
    .form-group { margin-bottom: 25px; }
    label { font-size: 11px; font-weight: 700; color: rgba(255,255,255,0.6); margin-bottom: 10px; display: block; text-transform: uppercase; letter-spacing: 1px; }
    input[type='text'], input[type='password'] {
      width: 100%; max-width: 400px; padding: 18px 20px; border-radius: 14px;
      background: rgba(0,0,0,0.3); color: #fff; font-size: 16px; outline: none; transition: 0.3s;
      border: 1px solid rgba(255,255,255,0.05); box-shadow: inset 0 2px 5px rgba(0,0,0,0.5);
    }
    input:focus { border-color: rgba(0,210,255,0.5); background: rgba(0,0,0,0.5); box-shadow: 0 0 15px rgba(0,210,255,0.2), inset 0 2px 5px rgba(0,0,0,0.7); }
    
    button[type='submit'] {
      padding: 16px 30px; background: linear-gradient(135deg, #00d2ff, #3a7bd5); width: 100%; max-width: 400px;
      color: white; border: none; border-radius: 14px; cursor: pointer; box-shadow: 0 5px 15px rgba(58,123,213,0.3);
      font-weight: 800; font-size: 13px; transition: all 0.3s; text-transform: uppercase; letter-spacing: 1px;
    }
    button[type='submit']:hover { transform: translateY(-3px); box-shadow: 0 10px 20px rgba(58,123,213,0.5); filter: brightness(1.1); }
    button[type='submit']:active { transform: translateY(1px); }
    
    .current-val { color: #fff; font-family: monospace; font-size: 22px; background: rgba(0,210,255,0.15); padding: 8px 16px; border-radius: 10px; letter-spacing: 4px; border: 1px solid rgba(0,210,255,0.3); box-shadow: 0 0 20px rgba(0,210,255,0.1); }

    /* MOBILE RESPONSIVE CARD LAYOUT - EXTREME NARROW (250px) */
    @media (max-width: 550px) {
      table, thead, tbody, th, td, tr { display: block; border: none; }
      th { display: none; }
      tr { 
        background: rgba(0,0,0,0.2); border-radius: 16px; margin-bottom: 15px; 
        border: 1px solid rgba(255,255,255,0.05); padding: 12px 15px;
      }
      td { 
        display: flex; justify-content: space-between; align-items: center; 
        padding: 10px 0; font-size: 13px; text-align: right; 
        border-bottom: 1px solid rgba(255,255,255,0.03) !important; 
      }
      td:last-child { border-bottom: none !important; }
      td::before { 
        display: block; color: rgba(255,255,255,0.4); font-size: 10px; 
        text-transform: uppercase; font-weight: 800; letter-spacing: 1px; text-align: left; margin-right: 10px;
      }

      #hist td:nth-of-type(1)::before { content: "Il y a"; }
      #hist td:nth-of-type(2)::before { content: "UID"; }
      #hist td:nth-of-type(3)::before { content: "Statut"; }
      #hist td:nth-of-type(4)::before { content: "Action"; }

      #accr td:nth-of-type(1)::before { content: "UID"; }
      #accr td:nth-of-type(2)::before { content: "Action"; }

      .header { flex-direction: column; gap: 15px; }
      .logout { position: static; width: 100%; text-align: center; }
      .tabs { flex-direction: column; gap: 5px; }
      .tab-btn { width: 100%; border-radius: 12px; }
      td { flex-direction: column; align-items: flex-start; gap: 5px; text-align: left; }
      td::before { margin-bottom: 2px; color: #00d2ff; }
      .btn { width: 100%; }
    }
  </style>
</head>
<body>
  <div class="header liquid-glass">
    <h1>Dashboard</h1>
    <a href="/logout" class="logout">Déconnexion</a>
  </div>

  <div class="container">
    <div class="tabs">
      <button class="tab-btn active" onclick="showTab('hist', this)">Historique</button>
      <button class="tab-btn" onclick="showTab('accr', this)">Autorisés</button>
      <button class="tab-btn" onclick="showTab('code', this)">Clavier</button>
      <button class="tab-btn" onclick="showTab('params', this)">Sécurité</button>
    </div>

    <div id="hist" class="panel liquid-glass active">
      <h2>Derniers passages</h2>
      <table>
        <tr><th>Il y a</th><th>Identifiant Puce</th><th>Statut</th><th>Action</th></tr>
        %HISTORY_TABLE%
      </table>
    </div>

    <div id="accr" class="panel liquid-glass">
      <h2>Badges Accrédités</h2>
      <table>
        <tr><th>Identifiant Puce</th><th>Révocation</th></tr>
        %BADGE_TABLE%
      </table>
    </div>

    <div id="code" class="panel liquid-glass">
      <h2>Code Clavier Public</h2>
      <p style="margin-bottom:35px; text-align:center;"><span class="current-val">%CURRENT_ACCESS_CODE%</span></p>
      <form action="/accesscode" method="POST">
        <div class="form-group">
          <label>Nouveau code pin (4 chiffres)</label>
          <input type="text" name="newcode" maxlength="4" pattern="[0-9]{4}" placeholder="0000" required>
        </div>
        <button type="submit">Appliquer le code</button>
      </form>
    </div>

    <div id="params" class="panel liquid-glass">
      <h2>Mot de passe Web</h2>
      <form action="/pwd" method="POST">
        <div class="form-group">
          <label>Nouveau mot de passe admin</label>
          <input type="password" name="newpwd" placeholder="••••••••" required>
        </div>
        <button type="submit">Mettre à jour</button>
      </form>
    </div>
  </div>

  <script>
    function showTab(tabId, btn) {
      document.querySelectorAll('.panel').forEach(el => el.classList.remove('active'));
      document.querySelectorAll('.tab-btn').forEach(el => el.classList.remove('active'));
      document.getElementById(tabId).classList.add('active');
      btn.classList.add('active');
    }

    const currentMillis = %UPTIME_MS%;
    const now = Date.now();
    
    document.querySelectorAll('.time-calc').forEach(el => {
      const eventMillis = parseInt(el.getAttribute('data-millis'));
      const diffStr = ((currentMillis - eventMillis) / 1000).toFixed(0);
      let secs = parseInt(diffStr);
      if (secs < 60) el.innerText = secs + "s";
      else if (secs < 3600) el.innerText = Math.floor(secs/60) + "m " + (secs%60) + "s";
      else el.innerText = Math.floor(secs/3600) + "h " + Math.floor((secs%3600)/60) + "m";
    });
    
    document.querySelectorAll('#hist td:nth-of-type(2), #accr td:nth-of-type(1)').forEach(el => {
       if(!el.innerHTML.includes('<a')) el.classList.add('uid-txt');
    });
  </script>
</body>
</html>
)=====";

#endif