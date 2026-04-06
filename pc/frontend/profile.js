import { fetchAPI, requireAuth, setupNavbar } from './utils.js';

requireAuth();
setupNavbar();

window.updatePassword = async () => {
    const p = document.getElementById('profilePwd').value;
    if(!p) return;
    try {
        await fetchAPI('/auth/password', { method: 'POST', body: JSON.stringify({ newpwd: p }) });
        alert("Mot de passe mis à jour !");
        document.getElementById('profilePwd').value = '';
    } catch(e) { alert("Erreur: "+e.message); }
};

document.getElementById('btnUpdatePwd').addEventListener('click', window.updatePassword);
