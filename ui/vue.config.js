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
    proxy: {
      '^/getLog': {
        target: 'http://localhost:3000',
      },
    }
  },

  chainWebpack: config => {
    config.plugins.delete('preload');
    config.plugins.delete('prefetch');
    config.optimization.delete('splitChunks');

    config.plugin('html')
      .tap(args => {
        args[0].inject = false;
        args[0].minify = false;
        args[0].templateParameters = {
          CDN: process.env.CDN_URL || ''
        };
        return args;
      });
  }
};
