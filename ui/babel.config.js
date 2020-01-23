module.exports = {
  "presets": [
    ["@vue/app", { useBuiltIns: "entry" }]
  ],
  "plugins": [
    [
      "transform-imports",
      {
        "quasar": {
          "transform": require("quasar/dist/babel-transforms/imports.js"),
          "preventFullImport": false
        }
      }
    ]
  ]
};
