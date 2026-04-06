import { fetchAPI, requireAuth, setupNavbar } from './utils.js';

requireAuth();
setupNavbar();

const urlParams = new URLSearchParams(window.location.search);
const currentLockerId = urlParams.get('id');

if (!currentLockerId) {
    window.location.href = '/';
}

const renderLocker = async () => {
    const fb = document.getElementById('lockerFeedback');
    const content = document.getElementById('lockerContent');

    fb.style.display = 'block';
    content.style.display = 'none';
    fb.innerText = "Chargement du casier...";

    try {
        const data = await fetchAPI(`/lockers/${currentLockerId}`);
        const l = data.locker;

        document.getElementById('lockerName').innerText = l.name;
        document.getElementById('lockerIdTitle').innerText = '#' + l.id;
        document.getElementById('newKeypadCode').value = l.keypadCode;

        document.getElementById('lockerUsersList').innerHTML = l.users.length ? l.users.map(ul => `
            <div class="list-item">
                <span>${ul.user.login}</span>
                <button class="btn btn-danger" onclick="window.removeUserFromLocker(${ul.userId})">✕</button>
            </div>
        `).join('') : '<p class="text-muted">Aucun utilisateur.</p>';

        document.getElementById('lockerBadgesList').innerHTML = l.badges.length ? l.badges.map(b => `
            <div class="list-item">
                <div>
                    <span style="font-family: monospace;" class="font-semibold">${b.name || 'Badge sans nom'}</span>
                    <span class="text-muted text-sm ml-2">(${b.uid})</span>
                </div>
                <div class="flex gap-2">
                    <button class="btn btn-outline" onclick="window.renameBadge('${b.uid}')">Nommer</button>
                    <button class="btn btn-danger" onclick="window.toggleBadge('${b.uid}')">Révoquer</button>
                </div>
            </div>
        `).join('') : '<p class="text-muted">Aucun badge.</p>';

        const accreditedUIDs = l.badges.map(b => b.uid);

        document.getElementById('lockerHistoryBody').innerHTML = l.logs.length ? l.logs.map(log => {
            const isCurrentlyAuth = accreditedUIDs.includes(log.uid);
            const statusDisplay = log.success;
            const nameDisplay = log.badgeName ? ` <span class="text-muted">(${log.badgeName})</span>` : '';
            return `
            <tr>
                <td>${new Date(log.timestamp).toLocaleString('fr-FR')}</td>
                <td style="font-family: monospace;">${log.uid}${nameDisplay}</td>
                <td><span class="status ${statusDisplay ? 'status-success' : 'status-error'}">${statusDisplay ? 'Autorisé' : 'Refusé'}</span></td>
                <td>
                    <button class="btn ${isCurrentlyAuth ? 'btn-danger' : 'btn-outline'}" onclick="window.toggleBadge('${log.uid}')" style="padding: 0.25rem 0.5rem; font-size: 0.75rem;">
                        ${isCurrentlyAuth ? 'Révoquer' : 'Accréditer'}
                    </button>
                </td>
            </tr>
            `;
        }).join('') : '<tr><td colspan="4" class="text-muted text-center">Aucun log récent.</td></tr>';

        fb.style.display = 'none';
        content.style.display = 'flex';
    } catch (e) {
        fb.innerHTML = `<span class="form-error">Erreur: ${e.message}</span>`;
    }
};

window.remoteUnlockCurrent = async () => {
    try {
        await fetchAPI(`/lockers/${currentLockerId}/remote-unlock`, { method: 'POST' });
        alert("Demande d'ouverture envoyée !");
    } catch (e) { alert("Erreur: " + e.message); }
};
document.getElementById('btnUnlock').addEventListener('click', window.remoteUnlockCurrent);

window.searchUsers = async (q) => {
    const resDiv = document.getElementById('search-results');
    if (!q) { resDiv.style.display = 'none'; return; }
    try {
        const data = await fetchAPI(`/users/search?q=${encodeURIComponent(q)}`);
        if (data.users && data.users.length) {
            resDiv.innerHTML = data.users.map(u => `
                <div class="search-item" onclick="window.addUserToLocker(${u.id})">${u.login}</div>
            `).join('');
            resDiv.style.display = 'block';
        } else {
            resDiv.innerHTML = '<div class="p-2 text-muted" style="padding: 0.5rem">Aucun résultat</div>';
            resDiv.style.display = 'block';
        }
    } catch { }
};
document.getElementById('searchInput').addEventListener('input', e => window.searchUsers(e.target.value));

window.addUserToLocker = async (userId) => {
    try {
        await fetchAPI(`/lockers/${currentLockerId}/users`, { method: 'POST', body: JSON.stringify({ userId }) });
        renderLocker();
        document.getElementById('search-results').style.display = 'none';
    } catch (e) { alert("Erreur: " + e.message); }
};

window.removeUserFromLocker = async (userId) => {
    try {
        await fetchAPI(`/lockers/${currentLockerId}/users/${userId}`, { method: 'DELETE' });
        renderLocker();
    } catch (e) { alert("Erreur: " + e.message); }
};

window.toggleBadge = async (uid) => {
    if (!uid || !uid.trim()) return;
    try {
        await fetchAPI(`/lockers/${currentLockerId}/badges`, { method: 'POST', body: JSON.stringify({ uid: uid.trim().toUpperCase() }) });
        renderLocker();
        document.getElementById('newBadgeUid').value = '';
    } catch (e) { alert("Erreur: " + e.message); }
};
document.getElementById('btnAddBadge').addEventListener('click', () => window.toggleBadge(document.getElementById('newBadgeUid').value));

window.updateKeypad = async (code) => {
    try {
        await fetchAPI(`/lockers/${currentLockerId}/config/accesscode`, { method: 'POST', body: JSON.stringify({ newcode: code }) });
        alert("Code Keypad sauvegardé");
    } catch (e) { alert("Erreur: " + e.message); }
};
document.getElementById('btnSaveKeypad').addEventListener('click', () => window.updateKeypad(document.getElementById('newKeypadCode').value));

window.renameLocker = async () => {
    const newName = prompt("Nouveau nom pour ce casier :");
    if (newName && newName.trim()) {
        try {
            await fetchAPI(`/lockers/${currentLockerId}/name`, { method: 'PUT', body: JSON.stringify({ name: newName.trim() }) });
            renderLocker();
        } catch (e) { alert("Erreur: " + e.message); }
    }
};

window.renameBadge = async (uid) => {
    const newName = prompt("Nouveau nom pour le badge " + uid + " :");
    if (newName !== null) {
        try {
            await fetchAPI(`/lockers/${currentLockerId}/badges/${uid}/name`, { method: 'PUT', body: JSON.stringify({ name: newName.trim() }) });
            renderLocker();
        } catch (e) { alert("Erreur: " + e.message); }
    }
};

loadLocker: renderLocker();
