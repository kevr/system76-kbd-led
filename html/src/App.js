import React, { useState } from 'react';
import logo from './logo.svg';
import { BlockPicker } from 'react-color';
import './App.css';

function App() {
  const [ color, setColor ] = useState("#ff0000");

  return (
    <div className="App">
      <BlockPicker
        color={color}
        onChangeComplete={e => {
          setColor(e.hex);
        }}
      />
    </div>
  );
}

export default App;
