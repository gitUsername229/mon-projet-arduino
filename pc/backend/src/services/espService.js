const prisma = require('../config/db');

class EspService {
    constructor() {
        this.espIps = {}; // lockerId -> ip
        this.lastSeen = {}; // lockerId -> Date.now() timestamp
        this.macToLocker = {}; // mac -> lockerId
    }

    setLockerMapping(mac, lockerId) {
        this.macToLocker[mac] = lockerId;
    }
    
    getLockerIdByMac(mac) {
        return this.macToLocker[mac];
    }

    setEspIp(lockerId, ip) {
        this.espIps[lockerId] = ip;
        this.markPresence(lockerId);
        console.log(`[EspService] ESP IP address registered for Locker ${lockerId}: ${ip}`);
        // Push config automatically when registered
        setTimeout(() => this.pushConfigToESP(lockerId), 2000);
    }
    
    markPresence(lockerId, ip = null) {
        this.lastSeen[lockerId] = Date.now();
        if (ip) {
            this.espIps[lockerId] = ip;
        }
    }
    
    isLockerOnline(lockerId) {
        if (!this.lastSeen[lockerId]) return false;
        // Check if last seen is within 15 seconds
        return (Date.now() - this.lastSeen[lockerId]) <= 15000;
    }

    async pushConfigToESP(lockerId, unlock = false) {
        const ip = this.espIps[lockerId];
        if (!ip) {
            console.error(`[EspService] Cannot push config. ESP IP is not registered for locker ${lockerId}.`);
            return;
        }

        try {
            const locker = await prisma.locker.findUnique({
                where: { id: lockerId },
                include: { badges: true }
            });
            
            if (!locker) return;
            
            const payload = {
                type: "config",
                accessCode: locker.keypadCode,
                badges: locker.badges.map(b => b.uid),
                unlock: unlock
            };
            
            const response = await fetch(`http://${ip}/config`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(payload)
            });

            if (!response.ok) {
                console.error(`[EspService] Failed to push config to locker ${lockerId}: HTTP ${response.status}`);
            } else {
                console.log(`[EspService] Config successfully pushed to Locker ${lockerId}`);
            }
        } catch(e) {
            console.error(`[EspService] Failed to push config to ESP (${lockerId}):`, e.message);
        }
    }
}

module.exports = new EspService();
