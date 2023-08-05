import React, { useEffect, useState } from 'react';
import logo from './logo.svg';
import './App.css';
import MyMap from './Map';
import axios from 'axios';


function App() {
  const [bounds, setBounds] = useState(null);
  const [tableData, setTableData] = useState(null);

  // This useEffect hook will fetch the data when the component is mounted.
  useEffect(() => {
    if (bounds) {
        console.log(bounds)
        // Make sure your backend accepts JSON in the request body.
        axios.post("http://localhost:8080/query", {coordinates: bounds.join(",")})
            .then((response) => {
                setTableData(response.data);
            })
            .catch((error) => {
                console.log(error);
            });
    }
}, [bounds]);


  return (
    <div className="App">
      <header className="App-header">
        <img src={logo} className="App-logo" alt="logo" />
        <p className="main-title">
          Wind Turbine Location Visualizer
        </p>
        <p className="instructions">
          Select the rectangle tool, and draw a region anywhere inside the United States. The data for all the wind turbines within your selected region will then be displayed for you below the map.
        </p>
        <MyMap bounds={bounds} setBounds={setBounds} />
        <a className="main-title">
          Wind Turbine Data
        </a>
        {tableData && 
  <table>
    <thead>
      <tr>
        <th>Earliest built</th>
        <th>Latest built</th>
        <th>Average capacity</th>
        <th>Tallest turbine</th>
        <th>Shortest turbine</th>
        <th>Average rotor diameter</th>
        <th>Turbine density</th>
        <th>Area of selection</th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td>{tableData["Earliest built"]}</td>
        <td>{tableData["Latest built"]}</td>
        <td>{tableData["Average capacity"]}</td>
        <td>{tableData["Tallest turbine"]}</td>
        <td>{tableData["Shortest turbine"]}</td>
        <td>{tableData["Average rotor diameter"]}</td>
        <td>{tableData["Turbine density"]}</td>
        <td>{tableData["Area of selection"]}</td>
      </tr>
    </tbody>
  </table>
}
      </header>
    </div>
  );

}

export default App;
