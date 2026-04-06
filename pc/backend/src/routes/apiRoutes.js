const express = require('express');
const router = express.Router();
const apiController = require('../controllers/apiController');
const authController = require('../controllers/authController');

// ESP Routes (no auth)
router.post('/esp/register', apiController.registerEsp);
router.post('/esp/heartbeat', apiController.receiveHeartbeat);
router.post('/esp/logs', apiController.receiveEspLogs);

// Auth Routes
router.post('/auth/login', authController.login);
router.get('/auth/me', authController.authenticate, authController.me);
router.post('/auth/register', authController.authenticate, authController.register); // Admin only internally handled
router.post('/auth/password', authController.authenticate, authController.updatePassword);

// User Search Route (for adding to lockers)
router.get('/users/search', authController.authenticate, apiController.searchUsers);

// Locker Routes (Auth required)
router.get('/lockers', authController.authenticate, apiController.getLockers);
router.get('/lockers/:id', authController.authenticate, apiController.getLockerDetails);
router.post('/lockers/:id/users', authController.authenticate, apiController.addUserToLocker);
router.delete('/lockers/:id/users/:userId', authController.authenticate, apiController.removeUserFromLocker);
router.post('/lockers/:id/badges', authController.authenticate, apiController.toggleBadge);
router.put('/lockers/:id/badges/:uid/name', authController.authenticate, apiController.updateBadgeName);
router.post('/lockers/:id/config/accesscode', authController.authenticate, apiController.updateAccessCode);
router.post('/lockers/:id/remote-unlock', authController.authenticate, apiController.remoteUnlock);
router.put('/lockers/:id/name', authController.authenticate, apiController.updateLockerName);

module.exports = router;
