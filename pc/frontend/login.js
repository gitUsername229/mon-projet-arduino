import { fetchAPI } from './utils.js';

document.getElementById('login-form').addEventListener('submit', async (e) => {
    e.preventDefault();
    const login = document.getElementById('loginUsername').value;
    const pass = document.getElementById('loginPassword').value;
    try {
        const data = await fetchAPI('/auth/login', { method: 'POST', body: JSON.stringify({ login, password: pass }) });
        if (data.error) throw new Error(data.error);
        
        localStorage.setItem('token', data.token);
        localStorage.setItem('user', JSON.stringify({ id: data.id, role: data.role, login: data.login }));
        
        window.location.href = '/';
    } catch (err) {
        document.getElementById('loginError').innerText = err.message || "Erreur de connexion";
    }
});
