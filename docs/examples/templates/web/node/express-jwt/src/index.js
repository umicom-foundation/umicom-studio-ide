import express from 'express'; import cors from 'cors'; import jwt from 'jsonwebtoken'; import dotenv from 'dotenv';
dotenv.config();
const app = express(); app.use(cors()); app.use(express.json());
const SECRET = process.env.JWT_SECRET || 'dev-secret-change-me';

app.post('/auth/login', (req,res)=>{ const { user, pass } = req.body || {}; if(user && pass){ const token = jwt.sign({ sub:user }, SECRET, { expiresIn: '1h' }); return res.json({ token }); } return res.status(400).json({ error:'missing creds' }); });
function auth(req,res,next){ const h=req.headers.authorization||''; const t=h.startsWith('Bearer ')?h.slice(7):null; if(!t) return res.status(401).json({error:'no token'}); try{ req.user = jwt.verify(t, SECRET); next(); } catch(e){ return res.status(401).json({error:'bad token'}); } }
app.get('/api/private', auth, (req,res)=> res.json({ ok:true, user:req.user }));

app.listen(4001, ()=> console.log('JWT API on http://localhost:4001'));
