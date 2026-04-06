const prisma = require('../config/db');
const bcrypt = require('bcrypt');
const jwt = require('jsonwebtoken');

const JWT_SECRET = process.env.JWT_SECRET || 'secret42';

exports.login = async (req, res) => {
    try {
        const { login, password } = req.body;
        if (!login || !password) return res.status(400).json({ error: "Missing login or password" });

        const user = await prisma.user.findUnique({ where: { login } });
        if (!user) return res.status(401).json({ error: "Invalid credentials" });

        const isMatch = await bcrypt.compare(password, user.password);
        if (!isMatch) return res.status(401).json({ error: "Invalid credentials" });

        const token = jwt.sign({ userId: user.id, role: user.role }, JWT_SECRET, { expiresIn: '1d' });
        res.json({ success: true, token, role: user.role, id: user.id, login: user.login });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};

exports.register = async (req, res) => {
    try {
        if (req.user.role !== 'ADMIN') return res.status(403).json({ error: "Forbidden" });

        const { login } = req.body;
        if (!login) return res.status(400).json({ error: "Missing login" });

        const existing = await prisma.user.findUnique({ where: { login } });
        if (existing) return res.status(400).json({ error: "User already exists" });

        // Generate a random 6-char alphanumeric+special password
        const chars = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*';
        let password = '';
        for(let i=0; i<6; i++) {
            password += chars.charAt(Math.floor(Math.random() * chars.length));
        }

        const hashed = await bcrypt.hash(password, 10);
        const newUser = await prisma.user.create({
            data: { login, password: hashed, role: 'USER' }
        });

        res.json({ success: true, login: newUser.login, password }); // send back raw password only once to admin
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};

exports.updatePassword = async (req, res) => {
    try {
        const { newpwd } = req.body;
        if (!newpwd) return res.status(400).json({ error: "Missing new password" });

        // Admin can update their own password, Users can update their own
        const hashed = await bcrypt.hash(newpwd, 10);
        await prisma.user.update({
            where: { id: req.user.userId },
            data: { password: hashed }
        });

        res.json({ success: true });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};

exports.me = async (req, res) => {
    try {
        const user = await prisma.user.findUnique({ where: { id: req.user.userId }});
        if (!user) return res.status(404).json({ error: "User not found" });
        res.json({ success: true, user: { id: user.id, login: user.login, role: user.role } });
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
};

// Middleware
exports.authenticate = (req, res, next) => {
    const authHeader = req.headers.authorization;
    if (!authHeader) return res.status(401).json({ error: "No token provided" });

    const token = authHeader.split(' ')[1];
    if (!token) return res.status(401).json({ error: "Malformed token" });
    
    jwt.verify(token, JWT_SECRET, (err, decoded) => {
        if (err) return res.status(401).json({ error: "Invalid token" });
        req.user = decoded; // { userId, role, iat, exp }
        next();
    });
};
