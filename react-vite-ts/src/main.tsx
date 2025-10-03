import React from 'react';import { createRoot } from 'react-dom/client';
import { Provider } from 'react-redux'; import { store } from './store';
import { BrowserRouter, Routes, Route, Link } from 'react-router-dom';
import Home from './pages/Home'; import About from './pages/About';
const root = createRoot(document.getElementById('root')!);
root.render(<Provider store={store}><BrowserRouter>
<nav><Link to="/">Home</Link> | <Link to="/about">About</Link></nav>
<Routes><Route path="/" element={<Home/>}/><Route path="/about" element={<About/>}/></Routes>
</BrowserRouter></Provider>);
