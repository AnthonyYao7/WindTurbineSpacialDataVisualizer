import React, { useEffect, useState } from 'react';
import logo from './logo.svg';
import './App.css';
import MyMap from './Map';
import axios from 'axios';


function App() {
  const [tableData, setTableData] = useState(null);

  // This useEffect hook will fetch the data when the component is mounted.
  useEffect(() => {
    // Use axios or fetch to get the data from your server or database.
    // This is just a placeholder and should be replaced with your actual API call.
    axios.get("/api/your-endpoint")
      .then((response) => {
        setTableData(response.data);
      })
      .catch((error) => {
        console.log(error);
      });
  }, []);

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
        <MyMap />
        <a className="main-title">
          Wind Turbine Data
        </a>
        {tableData && 
          <table>
            <thead>
              <tr>
                <th>State</th>
                <th>County</th>
                <th>Year</th>
                <th>Turbine's Capacity</th>
                <th>Turbine's Hub Height</th>
                <th>Turbine's Rotor Diameter</th>
                <th>Swept Area</th>
                <th>Turbine's Total Height</th>
                <th>Project's Capacity</th>
                <th>Number of Turbines in Project</th>
                <th>Turbine's Latitude</th>
                <th>Turbine's Longitude</th>
              </tr>
            </thead>
            <tbody>
              {tableData.map((item, index) => (
                <tr key={index}>
                  <td>{item.state}</td>
                  <td>{item.county}</td>
                  <td>{item.year}</td>
                  <td>{item.turbineCapacity}</td>
                  <td>{item.turbineHubHeight}</td>
                  <td>{item.turbineRotorDiameter}</td>
                  <td>{item.sweptArea}</td>
                  <td>{item.turbineTotalHeight}</td>
                  <td>{item.projectCapacity}</td>
                  <td>{item.numberOfTurbinesInProject}</td>
                  <td>{item.turbineLatitude}</td>
                  <td>{item.turbineLongitude}</td>
                </tr>
              ))}
            </tbody>
          </table>
        }
      </header>
    </div>
  );

}

export default App;
