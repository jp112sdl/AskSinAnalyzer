const path = require('path');

module.exports = {
  filenameHashing: false,

  pluginOptions: {
    quasar: {
      treeShake: true
    }
  },

  transpileDependencies: [
    /[\\\/]node_modules[\\\/]quasar[\\\/]/
  ],

  devServer: {
    proxy: 'http://localhost:3000'
  },

  chainWebpack: config => {
    config.plugins.delete('preload');
    config.plugins.delete('prefetch');
    config.optimization.delete('splitChunks');

    config.resolve.alias
      .set('@components', path.join(__dirname, 'src/components'));

    config.plugin('html')
      .tap(args => {
        args[0].inject = process.env.NODE_ENV !== 'production';
        args[0].minify = false;
        args[0].templateParameters = {
          CDN: process.env.VUE_APP_CDN_URL || '',
          NODE_ENV: process.env.NODE_ENV,
        };
        return args;
      });
  }
};
