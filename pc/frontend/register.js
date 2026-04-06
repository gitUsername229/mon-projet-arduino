import { fetchAPI, requireAdmin, setupNavbar } from './utils.js';

requireAdmin();
setupNavbar();

window.createAccount = async () => {
    const l = document.getElementById('newLogin').value;
    if(!l) return;
    try {
        const data = await fetchAPI('/auth/register', { method: 'POST', body: JSON.stringify({ login: l }) });
        const resDiv = document.getElementById('registerResult');
        resDiv.innerHTML = `<span class="text-success">Compte créé !</span><br/><br/>Login: <b>${data.login}</b><br/>Mot de passe: <b>${data.password}</b><br/><br/><span class="text-muted text-sm">Veuillez copier ce mot de passe, il ne sera plus affiché.</span>`;
        resDiv.style.display = 'block';
        document.getElementById('newLogin').value = '';
    } catch(e) { alert("Erreur: "+e.message); }
};

document.getElementById('btnCreate').addEventListener('click', window.createAccount);
