import { createRouter, createWebHistory } from 'vue-router';
import Home from './pages/Home.vue'; import Login from './pages/Login.vue';
import { useAuth } from './stores/auth';
const r = createRouter({ history:createWebHistory(), routes:[{path:'/',component:Home, meta:{auth:true}},{path:'/login',component:Login}] });
r.beforeEach((to)=>{ const a=useAuth(); if(to.meta.auth && !a.token) return '/login'; });
export default r;
