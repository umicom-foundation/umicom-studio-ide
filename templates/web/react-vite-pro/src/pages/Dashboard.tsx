import React from 'react'; import { useHelloQuery } from '../services/api';
export default function Dashboard(){ const { data } = useHelloQuery(); return <main><h1>Dashboard</h1><pre>{JSON.stringify(data,null,2)}</pre></main>; }
