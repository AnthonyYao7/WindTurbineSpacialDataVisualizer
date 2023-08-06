import React, { useEffect, useState } from 'react';
import logo from './logo.svg';
import './App.css';
import MyMap from './Map';
import axios from 'axios';


function App() {
  const [bounds, setBounds] = useState(null);
  const [tableData, setTableData] = useState(null);

  // This useEffect hook will post the bounds to the app server when the user selects a rectangle
  useEffect(() => {
    if (bounds) {
        axios.post("http://localhost:8080/query", {coordinates: bounds.join(",")})
            .then((response) => {
                setTableData(response.data); // sets the table with the response data
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
      <div className="table-container">
        {tableData && 
          <table>
            <thead>
              <tr>
                <th>Number of Turbines</th>
                <th>Area of selection (sq. mi.)</th>
                <th>Turbine density</th>
                <th>Earliest built</th>
                <th>Latest built</th>
                <th>Tallest turbine (m)</th>
                <th>Shortest turbine (m)</th>
                <th>Average capacity (KW)</th>
                <th>Average rotor diameter (m)</th>
                <th>Range tree query took (ms)</th>
                <th>K-d tree query took (ms)</th>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td>{tableData["Number of Turbines"]}</td>
                <td>{tableData["Area of selection"]}</td>
                <td>{tableData["Turbine density"]}</td>
                <td>{tableData["Earliest built"]}</td>
                <td>{tableData["Latest built"]}</td>
                <td>{tableData["Tallest turbine"]}</td>
                <td>{tableData["Shortest turbine"]}</td>
                <td>{tableData["Average capacity"]}</td>
                <td>{tableData["Average rotor diameter"]}</td>
                <td>{tableData["Range tree query took"]}</td>
                <td>{tableData["K-d tree query took"]}</td>
              </tr>
            </tbody>
          </table>
        }
      </div>
    </header>
  </div>
);



}
export default App;
