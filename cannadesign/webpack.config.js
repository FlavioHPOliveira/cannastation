const path = require('path')

module.exports = {
  mode: 'development',
  // The entry point file described above
  // entry: './src/index.js',
  entry: {
    login: './src/js/login.js',
    station: ['./src/js/station.js', './src/js/modal.js'],
    product: './src/js/product.js'
  },
  // The location of the build folder described above
  output: {
    path: path.resolve(__dirname, 'dist'),
    filename: '[name].bundle.js'
  },
  // Optional and for development only. This provides the ability to
  // map the built code back to the original source format when debugging.
  devtool: 'eval-source-map',
};