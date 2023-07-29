import logo from './logo.svg';
import './App.css';
import React from 'react';
import MyMap from './Map';

function App() {
  return (
    <div className="App">
      <header className="App-header">
        <img src={logo} className="App-logo" alt="logo" />
        <p>
          Wind Turbine Location Visualizer
        </p>
        <MyMap />
        <a>
          Click anywhere on the map to set the latitude + longitude.
        </a>
      </header>
    </div>
  );
}

export default App;
