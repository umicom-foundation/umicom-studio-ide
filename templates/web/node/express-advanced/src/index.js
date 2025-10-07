import express from 'express'; import cors from 'cors'; import dotenv from 'dotenv';
dotenv.config(); const app = express(); const PORT = process.env.PORT || 4000;
app.use(cors()); app.use(express.json());

app.get('/api/hello', (req,res)=> res.json({ ok:true, message:'Hello from advanced Express!' }));
app.use((err,req,res,next)=>{ console.error(err); res.status(500).json({ ok:false, error:'Internal error' }); });

app.listen(PORT, ()=> console.log(`API on http://localhost:${PORT}`));
