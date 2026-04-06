const prisma = require('../config/db');
const espService = require('../services/espService');

// =======================
// ESP32 endpoints
// =======================
exports.registerEsp = async (req, res) => {
    try {
        const { ip, mac } = req.body;
        if (!ip || !mac) return res.status(400).json({ error: "Missing ip or mac" });

        let locker = await prisma.locker.findUnique({ where: { espMac: mac } });
        if (!locker) {
            locker = await prisma.locker.create({ data: { name: `Locker ${mac.substring(12)}`, espMac: mac, espIp: ip } });
        } else {
            locker = await prisma.locker.update({ where: { id: locker.id }, data: { espIp: ip } });
        }

        espService.setLockerMapping(mac, locker.id);
        espService.setEspIp(locker.id, ip);
        res.json({ success: true, lockerId: locker.id });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};

exports.receiveHeartbeat = async (req, res) => {
    try {
        const { ip, mac } = req.body;
        if (!mac) return res.status(400).json({ error: "Missing mac" });
        
        let lockerId = espService.getLockerIdByMac(mac);
        
        // If not in memory, try to find in DB or create it
        if (!lockerId) {
            let locker = await prisma.locker.findUnique({ where: { espMac: mac } });
            if (!locker) {
                locker = await prisma.locker.create({ data: { name: `Locker ${mac.substring(12)}`, espMac: mac, espIp: ip } });
            } else if (locker.espIp !== ip) {
                locker = await prisma.locker.update({ where: { id: locker.id }, data: { espIp: ip } });
            }
            lockerId = locker.id;
            espService.setLockerMapping(mac, lockerId);
        }
        
        if (lockerId) {
            espService.markPresence(lockerId, ip);
        }
        
        res.json({ success: true });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
}

exports.receiveEspLogs = async (req, res) => {
    try {
        const { data, mac } = req.body;
        if (!mac) return res.status(400).json({ error: "Missing mac" });
        const lockerId = espService.getLockerIdByMac(mac);
        if (!lockerId) return res.status(404).json({ error: "Unregistered MAC" });

        if (data && Array.isArray(data)) {
            const mapped = data.map(l => ({ uid: l.uid, success: l.success, lockerId: lockerId }));
            await prisma.log.createMany({ data: mapped });
        }
        res.json({ success: true });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};

// =======================
// Lockers endpoints (Auth required)
// =======================

exports.getLockers = async (req, res) => {
    try {
        let lockers = [];
        if (req.user.role === 'ADMIN') {
            lockers = await prisma.locker.findMany();
        } else {
            const userLockers = await prisma.userLocker.findMany({
                where: { userId: req.user.userId },
                include: { locker: true }
            });
            lockers = userLockers.map(ul => ul.locker);
        }

        // Only return lockers that are currently connected via Heartbeat
        const connectedLockers = lockers.filter(l => espService.isLockerOnline(l.id));

        res.json({ success: true, lockers: connectedLockers });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};

exports.getLockerDetails = async (req, res) => {
    try {
        const lockerId = parseInt(req.params.id);
        const locker = await prisma.locker.findUnique({
            where: { id: lockerId },
            include: {
                users: { include: { user: true } },
                badges: true,
                logs: { orderBy: { timestamp: 'desc' }, take: 20 }
            }
        });
        if (!locker) return res.status(404).json({ error: "Locker not found" });

        // Access check
        if (req.user.role !== 'ADMIN') {
            const hasAccess = locker.users.some(ul => ul.userId === req.user.userId);
            if (!hasAccess) return res.status(403).json({ error: "Access denied" });
        }

        // Attach badge names to logs
        const badgesMap = new Map();
        locker.badges.forEach(b => badgesMap.set(b.uid, b));
        const augmentedLogs = locker.logs.map(log => ({
            ...log,
            badgeName: badgesMap.has(log.uid) ? badgesMap.get(log.uid).name : null
        }));
        
        res.json({ success: true, locker: { ...locker, logs: augmentedLogs } });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};

exports.addUserToLocker = async (req, res) => {
    try {
        const lockerId = parseInt(req.params.id);
        const { userId } = req.body;
        await prisma.userLocker.create({
            data: { lockerId, userId }
        });
        res.json({ success: true });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};

exports.removeUserFromLocker = async (req, res) => {
    try {
        const lockerId = parseInt(req.params.id);
        const userId = parseInt(req.params.userId);
        await prisma.userLocker.delete({
            where: { userId_lockerId: { userId, lockerId } }
        });
        res.json({ success: true });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};

exports.searchUsers = async (req, res) => {
    try {
        const q = req.query.q || '';
        const users = await prisma.user.findMany({
            where: {
                login: { contains: q, not: 'admin' }
            },
            take: 10,
            select: { id: true, login: true }
        });
        res.json({ success: true, users });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};

exports.toggleBadge = async (req, res) => {
    try {
        const lockerId = parseInt(req.params.id);
        const { uid } = req.body;
        if (!uid) return res.status(400).json({ error: "Missing uid" });

        const existing = await prisma.authorizedBadge.findUnique({
            where: { uid_lockerId: { uid, lockerId } }
        });

        if (existing) {
            await prisma.authorizedBadge.delete({ where: { uid_lockerId: { uid, lockerId } } });
        } else {
            await prisma.authorizedBadge.create({ data: { uid, lockerId } });
        }

        espService.pushConfigToESP(lockerId);
        res.json({ success: true });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};

exports.updateAccessCode = async (req, res) => {
    try {
        const lockerId = parseInt(req.params.id);
        const { newcode } = req.body;
        if (!newcode) return res.status(400).json({ error: "Missing newcode" });

        await prisma.locker.update({ where: { id: lockerId }, data: { keypadCode: newcode } });
        espService.pushConfigToESP(lockerId);
        res.json({ success: true });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};

exports.remoteUnlock = async (req, res) => {
    try {
        const lockerId = parseInt(req.params.id);
        espService.pushConfigToESP(lockerId, true);
        res.json({ success: true, message: "Unlock event dispatched" });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};

exports.updateLockerName = async (req, res) => {
    try {
        const lockerId = parseInt(req.params.id);
        const { name } = req.body;
        if (!name) return res.status(400).json({ error: "Missing name" });

        await prisma.locker.update({ where: { id: lockerId }, data: { name } });
        res.json({ success: true });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};

exports.updateBadgeName = async (req, res) => {
    try {
        const lockerId = parseInt(req.params.id);
        const uid = req.params.uid;
        const { name } = req.body;

        await prisma.authorizedBadge.update({ 
            where: { uid_lockerId: { uid, lockerId } }, 
            data: { name: name || null } 
        });
        res.json({ success: true });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};
