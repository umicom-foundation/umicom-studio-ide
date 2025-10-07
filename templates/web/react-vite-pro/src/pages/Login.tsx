import React, { useState } from 'react'; import { useNavigate } from 'react-router-dom'; import { store } from '../app/store'; import { setToken } from '../features/auth/authSlice';
export default function Login(){
  const [u,setU]=useState(''); const [p,setP]=useState(''); const nav=useNavigate();
  const onSubmit=(e:any)=>{ e.preventDefault(); if(u&&p){ store.dispatch(setToken('demo-token')); nav('/'); } };
  return (<form onSubmit={onSubmit}><h1>Login</h1><input placeholder="user" value={u} onChange={e=>setU(e.target.value)}/><input placeholder="pass" type="password" value={p} onChange={e=>setP(e.target.value)}/><button>Sign in</button></form>);
}
