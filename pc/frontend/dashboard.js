import { fetchAPI, requireAuth, setupNavbar } from './utils.js';

requireAuth();
setupNavbar();

const loadDashboard = async () => {
    const fb = document.getElementById('dashboardFeedback');
    const div = document.getElementById('dashboardLockers');

    try {
        const data = await fetchAPI('/lockers');
        const lockers = data.lockers || [];
        fb.innerText = "";

        if (lockers.length === 0) {
            fb.innerText = "Aucun casier n'est actuellement connecté au réseau.";
            return;
        }

        div.innerHTML = lockers.map(l => `
            <div class="glass-panel item-card" onclick="window.location.href='/locker.html?id=${l.id}'">
                <div class="item-card-title">${l.name}</div>
                <div class="text-sm text-muted">ID: ${l.id}</div>
            </div>
        `).join('');
    } catch (e) {
        fb.innerHTML = `<span class="form-error">Erreur de chargement: ${e.message}</span>`;
    }
};

loadDashboard();
