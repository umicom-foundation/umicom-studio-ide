// A tiny React component with basic state and a button.
import React, { useState } from 'react'

export default function App(){
  const [count, setCount] = useState(0) // State variable that starts at 0
  return (
    <main style={{fontFamily:'system-ui, sans-serif', margin:'2rem'}}>
      <h1 style={{color:'#2a5'}}>Umicom — React + Vite</h1>
      <p>This is a minimal React starter configured for Vite.</p>
      <button onClick={()=>setCount(c=>c+1)}>Clicked {count} times</button>
    </main>
  )
}
