import React, { useEffect, useState, memo, useRef } from 'react';
import { MapContainer, TileLayer, useMap } from 'react-leaflet';
import L from 'leaflet';
import 'leaflet-draw/dist/leaflet.draw.css';
import 'leaflet-draw';
import axios from 'axios';
import leafletPip from 'leaflet-pip';
import './Map.css';

let drawnLayers = new L.FeatureGroup();


const MyMap = memo(function Map({ bounds, setBounds }) {
  const [drawControl, setDrawControl] = useState(null);
  const [errorMessage, setErrorMessage] = useState('');
  const geoJsonLayerRef = useRef(null); // Create a ref to store the GeoJSON layer

  const MyComponent = () => {
    const map = useMap();

    useEffect(() => {
      if (!drawControl) {
        const control = new L.Control.Draw({
          draw: {
            polyline: false,
            polygon: false,
            circle: false,
            marker: false,
            rectangle: true,
          },
        });
        setDrawControl(control);
        map.addControl(control);
      }
    }, []);

    useEffect(() => {
      axios.get(process.env.PUBLIC_URL + '/data/custom.geo.json')
      .then(res => {
        if (geoJsonLayerRef.current) {
          map.removeLayer(geoJsonLayerRef.current);
        }


        geoJsonLayerRef.current = L.geoJSON(res.data);
        geoJsonLayerRef.current.addTo(map);

        // Remove all previous layers when the user starts a new drawing
        map.on(L.Draw.Event.DRAWSTART, function () {
          drawnLayers.eachLayer(function (layer) {
            map.removeLayer(layer);
          });
          drawnLayers = new L.FeatureGroup();
        });

        map.on(L.Draw.Event.CREATED, function (e) {
          const type = e.layerType;
          const layer = e.layer;
          setErrorMessage('');

          if (errorMessage === '' && type === 'rectangle') {
            const bounds = layer.getBounds();
            setBounds([bounds.getSouthWest().lng, bounds.getSouthWest().lat, bounds.getNorthEast().lng, bounds.getNorthEast().lat]);
        }
        
        
          if (type === 'rectangle') {
            const latlngs = layer.getLatLngs();
            let isInsideUS = false;
        
            latlngs[0].forEach(point => {
              const result = leafletPip.pointInLayer([point.lng, point.lat], geoJsonLayerRef.current, true);
              if(result.length !== 0) {
                isInsideUS = true;
              }
            });
        
            if (!isInsideUS) {
              setErrorMessage('All points of the drawn region fall outside the US. Please redraw the region.');
            }
          }
        
          // Add the drawn layer to the map and to the drawnLayers feature group
          drawnLayers.addLayer(layer);
          map.addLayer(layer);
        });
        
      });

      // Clean up when component unmounts
      return () => {
        map.off(L.Draw.Event.DRAWSTART);
        map.off(L.Draw.Event.CREATED);
      };

    }, []);

    return null;
  };

  return (
    <MapContainer
      center={[39.50, -98.35]}
      zoom={4}
      style={{ height: "100vh", width: "100%" }}
      dragging={false}
      zoomControl={true}
      touchZoom={true}
      doubleClickZoom={true}
      scrollWheelZoom={false}
    >
      <TileLayer
        attribution='&copy; <a href="http://osm.org/copyright">OpenStreetMap</a> contributors'
        url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
      />
      <MyComponent />
      {errorMessage && <div className="error-message">{errorMessage}</div>}
    </MapContainer>
  );
})

export default MyMap;
