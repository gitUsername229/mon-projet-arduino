export const API_URL = 'http://localhost:3000/api';
export let token = localStorage.getItem('token');
export let user = (() => { try { return JSON.parse(localStorage.getItem('user')); } catch { return null; } })();

export const fetchAPI = async (endpoint, options = {}) => {
  const headers = { 'Content-Type': 'application/json' };
  if (token) headers['Authorization'] = `Bearer ${token}`;
  
  const res = await fetch(`${API_URL}${endpoint}`, { ...options, headers });
  if (res.status === 401 && endpoint !== '/auth/login') {
    logout();
    throw new Error('Unauthorized');
  }
  return res.json();
};

export const logout = () => {
    localStorage.removeItem('token');
    localStorage.removeItem('user');
    window.location.href = '/login.html';
};

window.logout = logout;

export const requireAuth = () => {
    if (!token) window.location.href = '/login.html';
};

export const requireAdmin = () => {
    requireAuth();
    if (user && user.role !== 'ADMIN') window.location.href = '/';
};

export const setupNavbar = () => {
    const navUsername = document.getElementById('nav-username');
    if (navUsername && user) navUsername.innerText = user.login;
    const navRegister = document.getElementById('nav-btn-register');
    if (navRegister && user && user.role !== 'ADMIN') navRegister.style.display = 'none';
};

// Close dropdowns if clicking outside
window.addEventListener('click', (e) => {
    const menu = document.getElementById('profile-menu');
    if (menu && !menu.contains(e.target)) {
        menu.classList.remove('open');
    }
});
