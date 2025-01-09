import React from 'react';
import clsx from 'clsx';
import styles from './styles.module.css';

const FeatureList = [
  {
    title: 'Free and compatible with Blender',
    Img: './img/muscle.jpg',
    description: (
      <>
        Docusaurus lets you focus on your docs, and we&apos;ll do the chores. Go
        ahead and move your docs into the <code>docs</code> directory.
      </>
    ),
  },
  {
    title: 'Blender add-on to create musculo-squeletal system.',
    Img: require('@site/static/img/Skel.jpeg').default,
    description: (
      <>
        Simuscle is designed to be used with Blender. 
        I want Simuscle to bring musculo-squeletal simulation to Blender comunity.
      </>
    ),
  },
  {
    title: 'Human assets disponible and ready to use',
    Img: './img/Color muscle.jpg',
    description: (
      <>
        For testing and my personal uses, I set up a human body musculo-squeletal system, ready to simulate.
        Simuscle can be use for simulate any creature animation you want. 
        You will have to create the muscles and the squeleton.
      </>
    ),
  },
  {
    title: 'Technical aspect',
    Img: '.img/interface.jpg',
    description: (
      <>
        Simuscle is done using OpenGL, and Dear ImGui, in C++.
        The Geometry done in Blender will be imported to Simuscle.
        Once the computeation is done, the geometry is exported to .pc2 or .mdd so Blender will be able to load the solution in the cache.
      </>
    ),
  },
];

function Feature({Img, title, description}) {
  return (
    <div className={clsx('col col--6')}>
      <div className="text--center">
        <img className={styles.featureImg} src={Img} alt='Img' />
      </div>
      <div className="text--left padding-horiz--md">
        <h3 style={{paddingTop: "5%"}}>{title}</h3>
        <p className={styles.text}>{description}</p>
      </div>
    </div>
  );
}

export default function HomepageFeatures() {
  const Intro_title="What is Simuscle ?";
  const Intro_description="Simuscle is a software which relalise the computation part of the 3D Pipeline. It is centered around creature and character simulation. My goal is to gather all the require tools for making CFX (Creature/Character Visual Effect). For now, I focus on musculo-squeletal simulation but I also plan to integrated hair and cloth simulation in it. The input data, is an animation of a musculo-squeletal model of a creature, and Simuscle will compute the mesh cache to be render.";
  const Outro_title="Disclaimer";
  const Outro_description="The web-site is in progress, and some of the vidéo or photo aren't realated to Simuscle. The header video is a real film and is not done with simuscle. It allow me to visualize the kind of sytle I am looking for my web site."
  return (
    <section className={styles.features}>
      <div className="container">
        <div className="intro" style={{paddingTop: "3%"}}>
          <h3 className={styles.title_intro}>{Intro_title}</h3>
          <p className={styles.text_intro}>{Intro_description}</p>
        </div>
        <hr></hr>
        <div className="row" style={{paddingTop: "3%"}}>
          {FeatureList.map((props, idx) => (
            <Feature key={idx} {...props} />
          ))}
        </div>
        <hr></hr>
        <div className="outro" style={{paddingTop: "1%"}}>
          <h3 className={styles.title_intro}>{Outro_title}</h3>
          <p className={styles.text_intro}>{Outro_description}</p>
        </div>
      </div>
    </section>
  );
}
