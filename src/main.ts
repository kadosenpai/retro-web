import { mount } from 'svelte';
import App from './App.svelte';
import './ui/styles/global.css';

const target = document.getElementById('app');
if (!target) throw new Error('Không tìm thấy phần tử #app');

const app = mount(App, { target });

export default app;
