(function () { "use strict";
              
    // Access token - DO NOT CHANGE! GIS data.
    Cesium.Ion.defaultAccessToken = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiI5YWM4ZWVkZC01MGFjLTQ2ZGYtODZiMC1hNjFlNDczNTgwNjQiLCJpZCI6MTk5NDgsInNjb3BlcyI6WyJhc3IiLCJnYyJdLCJpYXQiOjE1NzY1MTA4Mjh9.7R9jmbXsv_vaj30UyyJ1h6N_hfBagXtjBsGiyQrky1Y';
    
    // Create Cesium viwever.
    var viewer = new Cesium.Viewer('cesiumContainer', {
      scene3DOnly: true,
      selectionIndicator: false,
      baseLayerPicker: false
    });
    
    //Remove base layer & set new Sentinel 2 layer.
    viewer.imageryLayers.remove(viewer.imageryLayers.get(0));
    viewer.imageryLayers.addImageryProvider(new Cesium.IonImageryProvider({ assetId : 3954 }));
    
    // Load Cesium World Terrain.
    viewer.terrainProvider = Cesium.createWorldTerrain({
        requestWaterMask : true, // required for water effects
        requestVertexNormals : true // required for terrain lighting
    });
    
    // Enable depth testing.
    viewer.scene.globe.depthTestAgainstTerrain = true;
    
    // Configure camera spawn.
    var initialPosition = new Cesium.Cartesian3.fromDegrees(15.637720, 46.531251, 1231.082799425431);
    var initialOrientation = new Cesium.HeadingPitchRoll.fromDegrees(7.1077496389876024807, -15, 0.025883251314954971306);
    var homeCameraView = {
        destination : initialPosition,
        orientation : {
            heading : initialOrientation.heading,
            pitch : initialOrientation.pitch,
            roll : initialOrientation.roll
        }
    };
              
    // Set spawn.
    viewer.scene.camera.setView(homeCameraView);
}());