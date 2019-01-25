// https://github.com/gulpjs/gulp/blob/master/docs/recipes/running-tasks-in-series.md
// https://www.npmjs.com/package/gulp-inline
var gulp = require('gulp'), 
    inline = require('gulp-inline'),
    // rename = require('gulp-rename');
    minifyJs = require('gulp-uglify'),
    minifyCss = require('gulp-minify-css'),
    minifyHTML = require('gulp-minify-html');
		gzip = require('gulp-gzip');
		del = require('del');
		jsonminify = require('gulp-jsonminify');

/* Clean destination folder */
gulp.task('clean', function() {
	return del(['dist/*']);
});

/* Copy static files */
gulp.task('files', function() {
	return gulp.src([
					'src/**/*.{jpg,jpeg,png,ico,gif}'
					// 'src/manifest.json',
					// 'src/service-worker.js',
					// 'src/precache-manifest.js'
			])
			.pipe(gulp.dest('dist/'));
});

gulp.task('jsonminify', function () {
	return gulp.src(['src/*.json'])
			.pipe(jsonminify())
			.pipe(gulp.dest('dist'));
});

gulp.task('minifyCss', () => {
	return gulp.src('src/css/*.css')
		.pipe(minifyCss())
		//.pipe(rename({
    //        suffix: '.min'
    //    }))
		.pipe(gulp.dest('dist/css'));
});

gulp.task('minifyJs', () => {
	return gulp.src('src/js/*.js')
		.pipe(minifyJs())
		//.pipe(rename({
    //  suffix: '.min'
    //}))
		.pipe(gulp.dest('dist/js'));
});

gulp.task('minifyJsSw', () => {
	return gulp.src('src/*.js')
		.pipe(minifyJs())
		//.pipe(rename({
    //  suffix: '.min'
    //}))
		.pipe(gulp.dest('dist'));
});

// gulp.task('inline', gulp.series('minifyCss', 'minifyJs'), () => {
gulp.task('inline', () => {
	var opts = {
    empy: true,
    spare: true
	};

	return gulp.src('src/index.html')
		.pipe(minifyHTML(opts))
		.pipe(inline({
			base: 'src/',
			js: minifyJs,
			css: minifyCss, // [minifyCss, autoprefixer({ browsers:['last 2 versions'] })],
			disabledTypes: ['svg', 'img'], // Only inline css/js files
			ignore: ['./service-worker.js', './precache-manifest.js']			
		}))
		//.pipe(gzip())
		.pipe(gulp.dest('dist'));
	});

gulp.task('minifyHtml', () => {
	var opts = {
    empy: true,
    spare: true
	};

	return gulp.src('src/index.html')
		.pipe(minifyHTML(opts))
		.pipe(gulp.dest('dist'));
});

gulp.task('default', gulp.series('clean', 'files', 'jsonminify', 'minifyJsSw', 'inline'));
//gulp.task('default', gulp.series('clean', 'files', 'minifyCss', 'minifyJs', 'minifyJsSw', 'inline'));

/* gulp.task('default', () => {
}) */
