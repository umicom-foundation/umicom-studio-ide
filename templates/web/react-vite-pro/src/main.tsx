import React from 'react'; import { createRoot } from 'react-dom/client';
import { Provider, useSelector } from 'react-redux'; import { store } from './app/store';
import { BrowserRouter, Routes, Route, Navigate, Link } from 'react-router-dom';
import Login from './pages/Login'; import Dashboard from './pages/Dashboard';
const Guard: React.FC<{children:React.ReactNode}> = ({children}) => {
  const token = (store.getState() as any).auth.token; return token ? <>{children}</> : <Navigate to="/login"/>;
};
const App = () => (<>
<nav><Link to="/">Dashboard</Link> | <Link to="/login">Login</Link></nav>
<Routes>
  <Route path="/" element={<Guard><Dashboard/></Guard>} />
  <Route path="/login" element={<Login/>} />
</Routes></>);
createRoot(document.getElementById('root')!).render(<Provider store={store}><BrowserRouter><App/></BrowserRouter></Provider>);
