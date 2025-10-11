// React entry point used by Vite to mount the App component.
import React from 'react'
import { createRoot } from 'react-dom/client'
import App from './App.jsx'

// Find the <div id="root"> placeholder in index.html.
const root = createRoot(document.getElementById('root'))
// Render our App into that placeholder.
root.render(<App />)
