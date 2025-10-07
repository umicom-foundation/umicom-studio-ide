import React from 'react'; import { useSelector, useDispatch } from 'react-redux';
import type { RootState } from '../store'; import { inc } from '../store';
export default function Home(){
  const v = useSelector((s:RootState)=> s.counter.value);
  const d = useDispatch();
  return <main><h1>Home</h1><p>Count: {v}</p><button onClick={()=>d(inc())}>+1</button></main>;
}
