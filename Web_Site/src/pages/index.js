import React from 'react';
import {useState, useRef, useEffect} from 'react';
import clsx from 'clsx';
import Link from '@docusaurus/Link';
import useDocusaurusContext from '@docusaurus/useDocusaurusContext';
import Layout from '@theme/Layout';
import HomepageFeatures from '@site/src/components/HomepageFeatures';

import styles from './index.module.css';

import classes from './video_test.module.css';

import image_file from '../../static/img/docusaurus.png'

function Size() {
  const size = {
    width: window.innerWidth,
    height: window.innerHeight,
  }
  return (
    <h2>{size.width} </h2>
  );
}

function HomepageHeader() {
  //const video_url = "http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/BigBuckBunny.mp4"
  const video_url = "./vid/Monkey_540.mp4"
  const {siteConfig} = useDocusaurusContext();
  // If window.innerWidth is too small, do not show some element after the title
  let [height, setHeight] = useState(0);
  const elementRef = useRef(null);

  useEffect(() => {
    setHeight(250+elementRef.current.clientHeight);
  }, []);

  return (
    <div className="Video_box">
      <video className={styles.video} autoPlay="autoplay" loop="loop" muted id="id-video-intro">
      <source src={video_url} type="video/mp4" />
      Your browser does not support the video tag.
      </video>
      <div className={styles.text_image}>{siteConfig.title}</div>
      <p className={styles.text_subtitle} ref={elementRef}>{siteConfig.tagline}</p>
        <Link
          className={styles.buttons}
          to="/docs/presentation">
          Get started
        </Link>
    </div>
  );
}

export default function Home() {
  const {siteConfig} = useDocusaurusContext();
  return (
    <Layout
      title={``}
      description="Description will go into a meta tag in <head />">
      <HomepageHeader />
      <main>
        <HomepageFeatures />
      </main>
    </Layout>
  );
}
