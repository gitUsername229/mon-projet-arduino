require('dotenv').config();
const express = require('express');
const cors = require('cors');
const prisma = require('./src/config/db');
const bcrypt = require('bcrypt');
const apiRoutes = require('./src/routes/apiRoutes');
const espService = require('./src/services/espService');

const app = express();
app.use(cors());
app.use(express.json());

// Initialize default admin and locker if they do not exist
async function initDefaults() {
  const adminCount = await prisma.user.count({ where: { role: 'ADMIN' } });
  if (adminCount === 0) {
    const hashed = await bcrypt.hash('admin', 10);
    await prisma.user.create({
      data: { login: 'admin', password: hashed, role: 'ADMIN' }
    });
    console.log("Default admin account created.");
  }
  
  const lockerCount = await prisma.locker.count();
  if (lockerCount === 0) {
    await prisma.locker.create({
      data: { name: 'Casier Principal', keypadCode: '0000' }
    });
    console.log("Default locker created.");
  }
}

initDefaults().then(() => {
    console.log("Initialization finished.");
});

// Configure API endpoint routes
app.use('/api', apiRoutes);

// Start Server
const PORT = process.env.PORT || 3000;
app.listen(PORT, '0.0.0.0', () => {
  console.log(`Backend API running on http://0.0.0.0:${PORT}`);
});
