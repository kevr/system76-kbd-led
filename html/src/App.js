import React, { useState, useEffect } from 'react';
import logo from './logo.svg';
import { BlockPicker } from 'react-color';
import ReactLoading from 'react-loading';
import './App.css';

function App() {
  // Keyboard state: a list of regions from left to right.
  const [ data, setData ] = useState(null);
  const [ pageWidth, setPageWidth ] =
    useState(document.documentElement.clientWidth);
  const [ keyboard, setKeyboard ] = useState(null);

  const onResize = (e) => {
    console.log('onResize');
    setPageWidth(document.documentElement.clientWidth);
  };

  // On component mount, get the initial colors.
  useEffect(() => {
    if(data === null) {
      let url = 'http://api.kevr.tk:8000/colors';
      const options = {
        method: 'GET',
        credentials: 'same-origin'
      };
      fetch(url, options).then((response) => {
        return response.json()
      }).then((json) => {
        setData(json);
      });
    }

    // Add resize listener.
    window.addEventListener('resize', onResize);

    // Cleanup.
    return () => {
      window.removeEventListener('resize', onResize);
    };
  });

  // On color picker change handler.
  const onChange = (e, region) => {
    const color = e.hex.replace('#', '');

    console.log(`Region: ${region}`);
    let url = `http://api.kevr.tk:8000/colors/${region}?color=${color}`;
    const options = {
      method: 'PUT',
      credentials: 'same-origin',
      headers: {
        'Content-Type': 'application/json'
      }
    };

    fetch(url, options);

    setData(Object.assign({}, data, {
      [region]: e.hex
    }));
  }
  console.log('Updating...');

  const makeStyle = (color) => {
    if(!color)
      return {};

    if(color[0] != '#')
      color = `#${color}`;

    return { backgroundColor: color };
  };

  // k: region key; left, center, right.
  const Region = ({ k, color }) => (
    <div key={k}
      className={`${k}Region flex cover`}
      style={makeStyle(color)}
    />
  );

  // Return our DOM to the user.
  return (
    <div className="App">
      {data === null ? (
        <div className="center">
          <ReactLoading color={"#3333ff"} />
        </div>
      ) : (
        <div className="flex">
          <div className="flex">
            <div className="flex" />
            <div className="keyboard regions flexRow" style={{
              width: pageWidth * 0.80,
              height: pageWidth * 0.80 * (9 / 21)
            }}>
              {Object.keys(data).slice(0, Object.keys(data).length - 1).map((k, i) => (
                <Region key={i} k={k} color={data[k]} />
              ))}
            </div>
            <div className="flex flexRow" style={{
              width: pageWidth * 0.80,
              margin: "0 auto"
            }}>
            {Object.keys(data).slice(0, Object.keys(data).length - 1).map((k, i) => (
              <BlockPicker
                key={i}
                className="colorPicker"
                color={data[k]}
                onChangeComplete={(e) => { onChange(e, k); }}
              />
            ))}
            </div>
            <div className="flex" />
          </div>
        </div>
      )}
    </div>
  );
}

export default App;
